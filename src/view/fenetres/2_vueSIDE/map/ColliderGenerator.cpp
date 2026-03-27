#include "ColliderGenerator.h"
#include "ofxOpenCv.h"
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>

void ColliderGenerator::generateAndSave(const ofImage& overlayImg, string outputPath) {
    if (!overlayImg.isAllocated()) return;
    
    int w = overlayImg.getWidth();
    int h = overlayImg.getHeight();
    
    ofPixels pixels = overlayImg.getPixels();
    ofxCvGrayscaleImage alphaImg;
    alphaImg.allocate(w, h);
    
    ofPixels grayPixels;
    grayPixels.allocate(w, h, OF_PIXELS_GRAY);
    
    // Extraction de la couche Alpha (ou Luminance globale si pas d'Alpha)
    int numChannels = pixels.getNumChannels();
    if (numChannels == 4) {
        for(int i = 0; i < w * h; i++) {
            grayPixels[i] = pixels[i * 4 + 3]; 
        }
    } else {
        for(int i = 0; i < w * h; i++) {
            grayPixels[i] = (pixels[i * numChannels] + pixels[i * numChannels + 1] + pixels[i * numChannels + 2]) / 3; 
        }
    }
    
    alphaImg.setFromPixels(grayPixels);
    alphaImg.threshold(40); // détail = Nettoyage du bruit, on garde ce qui est suffisamment opaque
    
    ofxCvContourFinder contourFinder;
    // Cherche les contours, bFindHoles = true pour récupérer les contours intérieurs et extérieurs
    contourFinder.findContours(alphaImg, 100, (w * h), 500, true);
    
    ofFbo fbo;
    fbo.allocate(w, h, GL_RGBA);
    fbo.begin();
    ofClear(0, 0, 0, 0); // Fond entièrement transparent
    
    ofSetColor(255, 255, 255, 255);
    ofSetLineWidth(16); // L'épaisseur des traits blancs = épaisseur des murs du collider
    
    for (int i = 0; i < contourFinder.blobs.size(); i++) {
        ofNoFill();
        ofBeginShape();
        for (int j = 0; j < contourFinder.blobs[i].pts.size(); j++) {
            ofVertex(contourFinder.blobs[i].pts[j].x, contourFinder.blobs[i].pts[j].y);
        }
        ofEndShape(true); // true permet de fermer la boucle des lignes
    }
    fbo.end();
    
    ofPixels outPix;
    fbo.readToPixels(outPix);
    ofSaveImage(outPix, outputPath);
}

void ColliderGenerator::saveFile(const ofPixels& pixels, string outputPath) {
    if (!pixels.isAllocated()) return;
    
    // Convertit le masque N&B (1 channel) en image RGBA avec fond transparent
    ofPixels outPix;
    outPix.allocate(pixels.getWidth(), pixels.getHeight(), OF_PIXELS_RGBA);
    int total = pixels.getWidth() * pixels.getHeight();
    for(int i = 0; i < total; i++) {
        unsigned char val = pixels[i];
        outPix[i*4 + 0] = 255;
        outPix[i*4 + 1] = 255;
        outPix[i*4 + 2] = 255;
        outPix[i*4 + 3] = val; // Le noir devient transparent, le blanc reste opaque
    }
    ofSaveImage(outPix, outputPath);
}



void ColliderGenerator::generateWithAI(const ofImage& inputImg, string outputPath) {
    if (!inputImg.isAllocated()) return;

    string modelPath = ofToDataPath("models/midas_small.onnx");
    if (!ofFile(modelPath).exists()) {
        ofLogWarning("ColliderGenerator") << "Modele IA introuvable : " << modelPath;
        ofLogWarning("ColliderGenerator") << "Veuillez telecharger la version OpenCV-Zoo de midas_small.onnx";
        return;
    }

    try {
        cv::dnn::Net net = cv::dnn::readNetFromONNX(modelPath);
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_DEFAULT);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

        int w = inputImg.getWidth();
        int h = inputImg.getHeight();

        // S'assurer que les pixels sont bien en RGB
        ofPixels rgbPixels = inputImg.getPixels();
        rgbPixels.setImageType(OF_IMAGE_COLOR); 
        
        ofxCvColorImage colorImg;
        colorImg.allocate(w, h);
        colorImg.setFromPixels(rgbPixels);
        cv::Mat cvImg = cv::cvarrToMat(colorImg.getCvImage());

        // Preparation du blob pour MiDaS Small
        cv::Mat blob = cv::dnn::blobFromImage(cvImg, 1.0 / 255.0, cv::Size(256, 256), cv::Scalar(123.675, 116.28, 103.53), false, false);
        net.setInput(blob);

        // Inference de profondeur
        cv::Mat output = net.forward();
        if(output.empty()) {
            ofLogError("ColliderGenerator") << "Erreur: Le modele n'a rien retourne.";
            return;
        }

        // Gestion ultra-securisee des dimensions de sortie (selon les modeles: 1x1xHxW ou 1xHxW ou HxW)
        int outH = 256, outW = 256;
        if (output.dims == 4) { outH = output.size[2]; outW = output.size[3]; }
        else if (output.dims == 3) { outH = output.size[1]; outW = output.size[2]; }
        else if (output.dims == 2) { outH = output.size[0]; outW = output.size[1]; }
        else {
            ofLogError("ColliderGenerator") << "Erreur: Format de sortie non gere (" << output.dims << " dimensions).";
            return;
        }

        cv::Mat depthMap(outH, outW, CV_32F, output.ptr<float>());
        
        if(depthMap.empty() || depthMap.rows == 0 || depthMap.cols == 0) {
            ofLogError("ColliderGenerator") << "Erreur: La carte de profondeur generee est vide.";
            return;
        }

        // Normalisation de la carte de profondeur (0 a 255)
        double minVal, maxVal;
        cv::minMaxLoc(depthMap, &minVal, &maxVal);
        
        if(minVal == maxVal) {
            ofLogError("ColliderGenerator") << "Erreur: Image de profondeur uniforme (impossible a normaliser).";
            return;
        }

        cv::Mat depth8U;
        depthMap.convertTo(depth8U, CV_8U, 255.0 / (maxVal - minVal), -minVal * 255.0 / (maxVal - minVal));
        
        // Redimensionnement vers la taille d'origine du calque en toute securite
        cv::resize(depth8U, depth8U, cv::Size(w, h));

        // Seuillage : On garde les objets/zones les plus "proches"
        cv::threshold(depth8U, depth8U, 150, 255, cv::THRESH_BINARY);

        ofPixels threshPix;
        threshPix.setFromExternalPixels(depth8U.ptr(), w, h, 1);
        
        saveFile(threshPix, outputPath);
        ofLogNotice("ColliderGenerator") << "Collider IA genere avec succes : " << outputPath;

    } catch(const cv::Exception& e) {
        ofLogError("ColliderGenerator") << "Erreur OpenCV DNN : " << e.what();
    }
}

void ColliderGenerator::generateWithSAM(const ofImage& inputImg, string outputPath) {
    if (!inputImg.isAllocated()) return;

    string modelPath = ofToDataPath("models/image_segmentation_efficientsam_ti_2025april.onnx");
    if (!ofFile(modelPath).exists()) {
        ofLogWarning("ColliderGenerator") << "Modele IA introuvable : " << modelPath;
        ofLogWarning("ColliderGenerator") << "Veuillez placer 'image_segmentation_efficientsam_ti_2025april.onnx' dans bin/data/";
        return;
    }

    try {
        cv::dnn::Net net = cv::dnn::readNetFromONNX(modelPath);
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_DEFAULT);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

        int w = inputImg.getWidth();
        int h = inputImg.getHeight();

        ofPixels rgbPixels = inputImg.getPixels();
        rgbPixels.setImageType(OF_IMAGE_COLOR); 
        
        ofxCvColorImage colorImg;
        colorImg.allocate(w, h);
        colorImg.setFromPixels(rgbPixels);
        cv::Mat cvImg = cv::cvarrToMat(colorImg.getCvImage());

        // 1. Preparation de l'Image (RGB, 1024x1024, pixels entre 0 et 1)
        cv::Mat image_blob = cv::dnn::blobFromImage(cvImg, 1.0 / 255.0, cv::Size(1024, 1024), cv::Scalar(0, 0, 0), false, false);
        
        // Trouver le centre de masse des traits pour y placer notre "Point" de generation automatique
        int fg_x = w / 2;
        int fg_y = h / 2;
        long sum_x = 0, sum_y = 0, count = 0;
        int numChannels = inputImg.getPixels().getNumChannels();
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                ofColor c = inputImg.getColor(x, y);
                if ((numChannels == 4 && c.a > 128) || (numChannels != 4 && c.getBrightness() > 128)) {
                    sum_x += x;
                    sum_y += y;
                    count++;
                }
            }
        }
        if (count > 0) {
            fg_x = sum_x / count;
            fg_y = sum_y / count;
        }

        // Mettre le point a l'echelle du modele 1024x1024
        float scaled_x = (float)fg_x * 1024.0f / w;
        float scaled_y = (float)fg_y * 1024.0f / h;

        // 2. Preparation des points et des labels (Le modele attend un batch max de 6 points)
        int max_points = 6;
        int shape_pts[4] = {1, 1, max_points, 2};
        cv::Mat points_blob(4, shape_pts, CV_32F, cv::Scalar(0));
        
        int shape_lbls[4] = {1, 1, max_points, 1};
        cv::Mat labels_blob(4, shape_lbls, CV_32F, cv::Scalar(-1)); // -1 = padding / ignore
        
        // On definit le premier point comme etant le premier plan (Foreground = 1.0)
        points_blob.ptr<float>()[0] = scaled_x;
        points_blob.ptr<float>()[1] = scaled_y;
        labels_blob.ptr<float>()[0] = 1.0f;

        // 3. Injection des 3 inputs dans le reseau
        net.setInput(image_blob, "batched_images");
        net.setInput(points_blob, "batched_point_coords");
        net.setInput(labels_blob, "batched_point_labels");

        // Inference de segmentation
        std::vector<cv::String> outNames = {"output_masks", "iou_predictions"};
        std::vector<cv::Mat> outputs;
        net.forward(outputs, outNames);

        if(outputs.size() < 2 || outputs[0].empty() || outputs[1].empty()) {
            ofLogError("ColliderGenerator") << "Erreur: Le modele SAM n'a pas retourne les masques et IOUs attendus.";
            return;
        }

        cv::Mat outputBlob = outputs[0];
        cv::Mat outputIou = outputs[1];

        // Chercher le masque ayant le meilleur score IOU predit
        int best_mask_idx = 0;
        float best_iou = -1.0f;
        int num_masks = outputIou.total(); 
        const float* iou_ptr = outputIou.ptr<float>();
        for(int i = 0; i < num_masks; ++i) {
            if(iou_ptr[i] > best_iou) {
                best_iou = iou_ptr[i];
                best_mask_idx = i;
            }
        }

        int dims = outputBlob.dims;
        int outH = outputBlob.size[dims - 2];
        int outW = outputBlob.size[dims - 1];

        // Pointer sur le masque selectionne
        const float* mask_ptr = outputBlob.ptr<float>() + best_mask_idx * (outH * outW);
        cv::Mat maskMap(outH, outW, CV_32F, (void*)mask_ptr);
        
        // Les valeurs >= 0 sont le foreground (Logits)
        cv::Mat mask8U;
        cv::threshold(maskMap, mask8U, 0.0, 255.0, cv::THRESH_BINARY);
        mask8U.convertTo(mask8U, CV_8U);

        // Redimensionnement a la taille de l'overlay original
        cv::resize(mask8U, mask8U, cv::Size(w, h), 0, 0, cv::INTER_NEAREST);

        ofPixels threshPix;
        threshPix.setFromExternalPixels(mask8U.ptr(), w, h, 1);
        
        saveFile(threshPix, outputPath);
        ofLogNotice("ColliderGenerator") << "Collider SAM genere avec succes : " << outputPath;

    } catch(const cv::Exception& e) {
        ofLogError("ColliderGenerator") << "Erreur inference SAM DNN : " << e.what();
    }
}

void ColliderGenerator::generateWithDexined(const ofImage& inputImg, string outputPath) {
    if (!inputImg.isAllocated()) return;

    string modelPath = ofToDataPath("models/edge_detection_dexined_2024sep.onnx");
    if (!ofFile(modelPath).exists()) {
        ofLogWarning("ColliderGenerator") << "Modele IA introuvable : " << modelPath;
        ofLogWarning("ColliderGenerator") << "Veuillez placer 'edge_detection_dexined_2024sep.onnx' dans bin/data/";
        return;
    }

    try {
        cv::dnn::Net net = cv::dnn::readNetFromONNX(modelPath);
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_DEFAULT);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

        int w = inputImg.getWidth();
        int h = inputImg.getHeight();

        ofPixels rgbPixels = inputImg.getPixels();
        rgbPixels.setImageType(OF_IMAGE_COLOR); 
        
        ofxCvColorImage colorImg;
        colorImg.allocate(w, h);
        colorImg.setFromPixels(rgbPixels);
        cv::Mat cvImg = cv::cvarrToMat(colorImg.getCvImage());

        // Dexined requiert generalement des images BGR selon les pre-entrainements standards (OpenCV load)
        cv::cvtColor(cvImg, cvImg, cv::COLOR_RGB2BGR);

        // Blob input : 512x512, scalaire specifique pour ce modele
        cv::Mat blob = cv::dnn::blobFromImage(cvImg, 1.0, cv::Size(512, 512), cv::Scalar(103.5, 116.2, 123.6), false, false, CV_32F);
        net.setInput(blob);

        std::vector<cv::Mat> outputs;
        net.forward(outputs, net.getUnconnectedOutLayersNames());

        if(outputs.empty()) {
            ofLogError("ColliderGenerator") << "Erreur: Le modele Dexined n'a rien retourne.";
            return;
        }

        // Post-Processing
        std::vector<cv::Mat> preds;
        for (const cv::Mat &p : outputs) {
            cv::Mat processed;
            // Si c'est un format (1, 1, H, W), on l'aplatit en (H, W)
            if (p.dims == 4 && p.size[0] == 1 && p.size[1] == 1) {
                processed = cv::Mat(p.size[2], p.size[3], p.type(), (void*)p.ptr<float>()).clone();
            } else {
                processed = p.clone();
            }

            // Activation Sigmoid
            cv::exp(-processed, processed);
            processed = 1.0 / (1.0 + processed);

            cv::Mat img;
            cv::normalize(processed, img, 0, 255, cv::NORM_MINMAX, CV_8U);
            cv::resize(img, img, cv::Size(w, h));
            preds.push_back(img);
        }

        // Le dernier element est generalement la version fusionnee (fuse map)
        cv::Mat edgeMap = preds.back();

        // Threshold pour obtenir des murs fermes au lieu d'un gris continu
        cv::threshold(edgeMap, edgeMap, 100, 255, cv::THRESH_BINARY);

        ofPixels threshPix;
        threshPix.setFromExternalPixels(edgeMap.ptr(), w, h, 1);
        
        saveFile(threshPix, outputPath);
        ofLogNotice("ColliderGenerator") << "Collider Dexined genere avec succes : " << outputPath;

    } catch(const cv::Exception& e) {
        ofLogError("ColliderGenerator") << "Erreur inference Dexined DNN : " << e.what();
    }
}
