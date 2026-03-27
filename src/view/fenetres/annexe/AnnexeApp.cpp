#include "AnnexeApp.h"
#include "ofxOpenCv.h"
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>

void AnnexeApp::setup() {
    ofSetBackgroundColor(30);
    
    // Centrage et dézoom initial pour voir le canevas en entier
    viewZoom = std::min((float)ofGetWidth() / targetWidth, (float)ofGetHeight() / targetHeight) * 0.95f;
    viewPan.x = (ofGetWidth() - targetWidth * viewZoom) / 2.0f;
    viewPan.y = (ofGetHeight() - targetHeight * viewZoom) / 2.0f;

    // SAM UI
    samSaveBtn.set(10, 60, 100, 25);
    samResetBtn.set(10, 90, 100, 25);
}

void AnnexeApp::update() {
    if (bRippleEffect && img.isAllocated()) {
        updateRipple();
    }
}

void AnnexeApp::draw() {
    ofBackground(30);
    
    ofPushMatrix();
    ofTranslate(viewPan.x, viewPan.y);
    ofScale(viewZoom, viewZoom);
    
    // Fond gris délimitant la zone ciblée
    ofSetColor(50);
    ofDrawRectangle(0, 0, targetWidth, targetHeight);
    
    if (img.isAllocated()) {
        ofSetColor(255);
        if (bRippleEffect && rippleOutputImage.isAllocated()) {
            rippleOutputImage.draw(0, 0, targetWidth, targetHeight);
        } else {
            img.draw(0, 0, targetWidth, targetHeight);
        }

        if (bSamControlActive) {
            // Draw SAM mask overlay
            if (bSamMaskGenerated && samPreviewMask.isAllocated()) {
                ofPushStyle();
                ofSetColor(255, 0, 0, 165); // Red overlay with ~65% opacity
                samPreviewMask.draw(0, 0, targetWidth, targetHeight);
                ofPopStyle();
            }

            // Draw points
            for (size_t i = 0; i < samPoints.size(); i++) {
                if (samLabels[i] != 0 && samLabels[i] != 1) { // box points, don't draw them individually
                    continue;
                }

                ofVec2f p = samPoints[i];
                // map point from image space to screen space
                float screenX = ofMap(p.x, 0, img.getWidth(), 0, targetWidth);
                float screenY = ofMap(p.y, 0, img.getHeight(), 0, targetHeight);

                ofPushStyle();
                if (samLabels[i] == 1) {
                    ofSetColor(0, 255, 0);
                } else { // background (samLabels[i] == 0)
                    ofSetColor(255, 0, 0);
                }
                ofFill();
                ofDrawCircle(screenX, screenY, 5 / viewZoom);
                ofPopStyle();
            }
            
            // Draw bounding boxes from points
            for (size_t i = 0; i < samPoints.size(); ++i) {
                if (samLabels[i] == 2 && i + 1 < samPoints.size() && samLabels[i+1] == 3) {
                    ofVec2f p1 = samPoints[i];
                    ofVec2f p2 = samPoints[i+1];
                    float screenX1 = ofMap(p1.x, 0, img.getWidth(), 0, targetWidth);
                    float screenY1 = ofMap(p1.y, 0, img.getHeight(), 0, targetHeight);
                    float screenX2 = ofMap(p2.x, 0, img.getWidth(), 0, targetWidth);
                    float screenY2 = ofMap(p2.y, 0, img.getHeight(), 0, targetHeight);
                    ofPushStyle();
                    ofNoFill();
                    ofSetColor(0, 0, 255);
                    ofSetLineWidth(2 / viewZoom);
                    ofDrawRectangle(screenX1, screenY1, screenX2 - screenX1, screenY2 - screenY1);
                    ofPopStyle();
                    i++; // Skip next point as it's part of the box
                }
            }
        }

    } else {
        // Cadre rouge pour bien voir la délimitation quand c'est vide
        ofNoFill();
        ofSetLineWidth(2);
        ofSetColor(255, 0, 0);
        ofDrawRectangle(0, 0, targetWidth, targetHeight);
        ofFill();
    }
    
    ofPopMatrix();

    // --- SAM Control UI ---
    if (bSamControlActive) {
        if (bSamIsDragging) {
            ofVec2f currentMouse = getTransformedMouse();
            ofPushMatrix();
            ofTranslate(viewPan.x, viewPan.y);
            ofScale(viewZoom, viewZoom);
            ofPushStyle();
            ofNoFill();
            ofSetColor(0, 0, 255, 150);
            ofSetLineWidth(1 / viewZoom);
            ofDrawRectangle(samDragStart.x, samDragStart.y, currentMouse.x - samDragStart.x, currentMouse.y - samDragStart.y);
            ofPopStyle();
            ofPopMatrix();
        }

        ofPushStyle();
        ofDrawBitmapStringHighlight("FG: Clic | BG: TAB+Clic | BOX: Drag", 120, 77, ofColor(0), ofColor(255));
        ofDrawBitmapStringHighlight("Sauver", samSaveBtn.x + 5, samSaveBtn.y + 17, ofColor(50, 180, 50), ofColor(255));
        ofDrawBitmapStringHighlight("Reset", samResetBtn.x + 5, samResetBtn.y + 17, ofColor(180, 50, 50), ofColor(255));
        ofPopStyle();
    }
    
    // --- Recording logic ---
    if (bIsRecording) {
        saveRecordedFrame();
        // Visual feedback for recording
        ofPushStyle();
        ofSetColor(255, 0, 0);
        ofFill();
        ofDrawCircle(20, 20, 10); // Red dot in top-left corner
        ofPopStyle();
    }

    // Message d'avertissement clignotant
    if (ofGetElapsedTimef() < warningEndTime) {
        if (sin(ofGetElapsedTimef() * 15.0f) > 0) { // Clignotement
            ofPushStyle();
            ofDrawBitmapStringHighlight(warningMessage, ofGetWidth() / 2.0f - warningMessage.length() * 4.0f, 50, ofColor(255, 0, 0), ofColor(255));
            ofPopStyle();
        }
    }
    
    // HUD Infos
    if (isSpacePressed) {
        ofDrawBitmapStringHighlight("ESPACE + DRAG: Pan | MOLETTE: Zoom | R: Reset", 10, ofGetHeight() - 20, ofColor(50, 200, 50), ofColor(0));
    } else {
        ofSetColor(255);
        ofDrawBitmapStringHighlight("ESPACE + DRAG: Pan | MOLETTE: Zoom | R: Reset", 10, ofGetHeight() - 20, ofColor(30, 30, 30, 200), ofColor(255));
    }
}

void AnnexeApp::saveHighResFrame() {
    ofFbo fbo;
    fbo.allocate(targetWidth, targetHeight, GL_RGB);
    
    fbo.begin();
    ofClear(50, 50, 50, 255); // Fond gris
    if (img.isAllocated()) {
        ofSetColor(255);
        if (bRippleEffect && rippleOutputImage.isAllocated()) {
            rippleOutputImage.draw(0, 0, targetWidth, targetHeight);
        } else {
            img.draw(0, 0, targetWidth, targetHeight);
        }
    } else {
        ofNoFill();
        ofSetLineWidth(2);
        ofSetColor(255, 0, 0);
        ofDrawRectangle(0, 0, targetWidth, targetHeight);
        ofFill();
    }
    fbo.end();

    ofPixels pix;
    fbo.readToPixels(pix);
    
    ofDirectory dir("export");
    if(!dir.exists()) dir.create(true);
    
    string filename = "export/annexe_frame_" + ofGetTimestampString() + ".jpg";
    ofSaveImage(pix, filename, OF_IMAGE_QUALITY_HIGH);
    ofLogNotice("AnnexeApp") << "Frame haute resolution sauvegardee : " << filename;
    
    warningMessage = "Image sauvegardee : " + filename;
    warningEndTime = ofGetElapsedTimef() + 3.0f; // Feedback visuel temporaire
}

void AnnexeApp::toggleRecording() {
    bIsRecording = !bIsRecording;
    if (bIsRecording) {
        recordFrameCount = 0;
        recordFolder = "export/annexe_rec_" + ofGetTimestampString();
        ofDirectory dir(recordFolder);
        if (!dir.exists()) {
            dir.create(true);
        }
        ofLogNotice("AnnexeApp") << "Debut de l'enregistrement video dans : " << recordFolder;
        warningMessage = "REC ON: " + recordFolder;
        warningEndTime = ofGetElapsedTimef() + 3.0f;
    } else {
        ofLogNotice("AnnexeApp") << "Fin de l'enregistrement video. " << recordFrameCount << " frames sauvegardees.";
        warningMessage = "REC OFF: " + ofToString(recordFrameCount) + " frames";
        warningEndTime = ofGetElapsedTimef() + 3.0f;
    }
}

void AnnexeApp::saveRecordedFrame() {
    ofFbo fbo;
    fbo.allocate(targetWidth, targetHeight, GL_RGB);
    
    fbo.begin();
    ofClear(50, 50, 50, 255);
    if (img.isAllocated()) {
        ofSetColor(255);
        if (bRippleEffect && rippleOutputImage.isAllocated()) {
            rippleOutputImage.draw(0, 0, targetWidth, targetHeight);
        } else {
            img.draw(0, 0, targetWidth, targetHeight);
        }
    } else {
        ofNoFill();
        ofSetLineWidth(2);
        ofSetColor(255, 0, 0);
        ofDrawRectangle(0, 0, targetWidth, targetHeight);
        ofFill();
    }
    fbo.end();

    ofPixels pix;
    fbo.readToPixels(pix);
    
    string filename = recordFolder + "/frame_" + ofToString(recordFrameCount, 6, '0') + ".jpg";
    ofSaveImage(pix, filename, OF_IMAGE_QUALITY_HIGH);
    
    recordFrameCount++;
}

void AnnexeApp::generateDepthMapAI() {
    if (!img.isAllocated()) {
        warningMessage = "Aucune image a analyser !";
        warningEndTime = ofGetElapsedTimef() + 3.0f;
        return;
    }

    string modelPath = ofToDataPath("models/midas_small.onnx");
    if (!ofFile(modelPath).exists()) {
        ofLogWarning("AnnexeApp") << "Modele IA introuvable : " << modelPath;
        warningMessage = "Modele IA introuvable !";
        warningEndTime = ofGetElapsedTimef() + 3.0f;
        return;
    }

    try {
        cv::dnn::Net net = cv::dnn::readNetFromONNX(modelPath);
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_DEFAULT);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

        int w = img.getWidth();
        int h = img.getHeight();

        ofPixels rgbPixels = img.getPixels();
        rgbPixels.setImageType(OF_IMAGE_COLOR); 
        
        ofxCvColorImage colorImg;
        colorImg.allocate(w, h);
        colorImg.setFromPixels(rgbPixels);
        cv::Mat cvImg = cv::cvarrToMat(colorImg.getCvImage());

        // Preparation du blob pour MiDaS Small
        cv::Mat blob = cv::dnn::blobFromImage(cvImg, 1.0 / 255.0, cv::Size(256, 256), cv::Scalar(123.675, 116.28, 103.53), false, false);
        net.setInput(blob);

        cv::Mat output = net.forward();
        if(output.empty()) return;

        int outH = 256, outW = 256;
        if (output.dims == 4) { outH = output.size[2]; outW = output.size[3]; }
        else if (output.dims == 3) { outH = output.size[1]; outW = output.size[2]; }
        else if (output.dims == 2) { outH = output.size[0]; outW = output.size[1]; }

        cv::Mat depthMap(outH, outW, CV_32F, output.ptr<float>());
        
        double minVal, maxVal;
        cv::minMaxLoc(depthMap, &minVal, &maxVal);
        
        if(minVal == maxVal) return;

        cv::Mat depth8U;
        depthMap.convertTo(depth8U, CV_8U, 255.0 / (maxVal - minVal), -minVal * 255.0 / (maxVal - minVal));
        
        cv::resize(depth8U, depth8U, cv::Size(w, h));
        cv::threshold(depth8U, depth8U, 150, 255, cv::THRESH_BINARY); // Même seuillage que ColliderGenerator

        ofPixels outPix;
        outPix.allocate(w, h, OF_PIXELS_RGBA);
        int total = w * h;
        for(int i = 0; i < total; i++) {
            unsigned char val = depth8U.data[i];
            outPix[i*4 + 0] = 255;
            outPix[i*4 + 1] = 255;
            outPix[i*4 + 2] = 255;
            outPix[i*4 + 3] = val; // Le noir devient transparent, le blanc reste opaque
        }
        
        ofDirectory dir("export");
        if(!dir.exists()) dir.create(true);
        
        string filename = "export/annexe_ai_depth_" + ofGetTimestampString() + ".png";
        ofSaveImage(outPix, filename);
        
        warningMessage = "Depth map generee : " + filename;
        warningEndTime = ofGetElapsedTimef() + 3.0f;
        ofLogNotice("AnnexeApp") << "Depth map AI generee avec succes : " << filename;

    } catch(const cv::Exception& e) {
        ofLogError("AnnexeApp") << "Erreur OpenCV DNN : " << e.what();
        warningMessage = "Erreur IA (OpenCV DNN)";
        warningEndTime = ofGetElapsedTimef() + 3.0f;
    }
}

void AnnexeApp::generateSAMCollider() {
    if (!img.isAllocated()) {
        warningMessage = "Aucune image a analyser !";
        warningEndTime = ofGetElapsedTimef() + 3.0f;
        return;
    }

    string modelPath = ofToDataPath("models/SAM/image_segmentation_efficientsam_ti_2025april_int8.onnx");
    if (!ofFile(modelPath).exists()) {
        ofLogWarning("AnnexeApp") << "Modele IA introuvable : " << modelPath;
        warningMessage = "Modele SAM introuvable !";
        warningEndTime = ofGetElapsedTimef() + 3.0f;
        return;
    }

    try {
        cv::dnn::Net net = cv::dnn::readNetFromONNX(modelPath);
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_DEFAULT);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

        int w = img.getWidth();
        int h = img.getHeight();

        ofPixels rgbPixels = img.getPixels();
        rgbPixels.setImageType(OF_IMAGE_COLOR); 
        
        ofxCvColorImage colorImg;
        colorImg.allocate(w, h);
        colorImg.setFromPixels(rgbPixels);
        cv::Mat cvImg = cv::cvarrToMat(colorImg.getCvImage());

        cv::Mat image_blob = cv::dnn::blobFromImage(cvImg, 1.0 / 255.0, cv::Size(1024, 1024), cv::Scalar(0, 0, 0), false, false);
        
        int fg_x = w / 2;
        int fg_y = h / 2;
        long sum_x = 0, sum_y = 0, count = 0;
        int numChannels = img.getPixels().getNumChannels();
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                ofColor c = img.getColor(x, y);
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

        float scaled_x = (float)fg_x * 1024.0f / w;
        float scaled_y = (float)fg_y * 1024.0f / h;

        int max_points = 6;
        int shape_pts[4] = {1, 1, max_points, 2};
        cv::Mat points_blob(4, shape_pts, CV_32F, cv::Scalar(0));
        
        int shape_lbls[4] = {1, 1, max_points, 1};
        cv::Mat labels_blob(4, shape_lbls, CV_32F, cv::Scalar(-1));
        
        points_blob.ptr<float>()[0] = scaled_x;
        points_blob.ptr<float>()[1] = scaled_y;
        labels_blob.ptr<float>()[0] = 1.0f;

        net.setInput(image_blob, "batched_images");
        net.setInput(points_blob, "batched_point_coords");
        net.setInput(labels_blob, "batched_point_labels");

        std::vector<cv::String> outNames = {"output_masks", "iou_predictions"};
        std::vector<cv::Mat> outputs;
        net.forward(outputs, outNames);

        if(outputs.size() < 2 || outputs[0].empty() || outputs[1].empty()) {
            ofLogError("AnnexeApp") << "Erreur: Le modele SAM n'a pas retourne les masques et IOUs attendus.";
            return;
        }

        cv::Mat outputBlob = outputs[0];
        cv::Mat outputIou = outputs[1];

        int best_mask_idx = 0;
        float best_iou = -1.0f;
        const float* iou_ptr = outputIou.ptr<float>();
        for(int i = 0; i < outputIou.total(); ++i) {
            if(iou_ptr[i] > best_iou) {
                best_iou = iou_ptr[i];
                best_mask_idx = i;
            }
        }

        int dims = outputBlob.dims;
        int outH = outputBlob.size[dims - 2];
        int outW = outputBlob.size[dims - 1];

        const float* mask_ptr = outputBlob.ptr<float>() + best_mask_idx * (outH * outW);
        cv::Mat maskMap(outH, outW, CV_32F, (void*)mask_ptr);
        
        cv::Mat mask8U;
        cv::threshold(maskMap, mask8U, 0.0, 255.0, cv::THRESH_BINARY);
        mask8U.convertTo(mask8U, CV_8U);

        cv::resize(mask8U, mask8U, cv::Size(w, h), 0, 0, cv::INTER_NEAREST);

        ofPixels outPix;
        outPix.allocate(w, h, OF_PIXELS_RGBA);
        int total = w * h;
        for(int i = 0; i < total; i++) {
            unsigned char val = mask8U.data[i];
            outPix[i*4 + 0] = 255;
            outPix[i*4 + 1] = 255;
            outPix[i*4 + 2] = 255;
            outPix[i*4 + 3] = val;
        }
        
        ofDirectory dir("export");
        if(!dir.exists()) dir.create(true);
        
        string filename = "export/annexe_sam_collider_" + ofGetTimestampString() + ".png";
        ofSaveImage(outPix, filename);
        
        warningMessage = "Collider SAM genere : " + filename;
        warningEndTime = ofGetElapsedTimef() + 3.0f;
        ofLogNotice("AnnexeApp") << "Collider SAM genere avec succes : " + filename;

    } catch(const cv::Exception& e) {
        ofLogError("AnnexeApp") << "Erreur inference SAM DNN : " << e.what();
        warningMessage = "Erreur IA (SAM DNN)";
        warningEndTime = ofGetElapsedTimef() + 3.0f;
    }
}

void AnnexeApp::toggleSamControl() {
    bSamControlActive = !bSamControlActive;
    if (bSamControlActive) {
        if (!img.isAllocated()) {
            warningMessage = "Chargez une image d'abord !";
            warningEndTime = ofGetElapsedTimef() + 3.0f;
            bSamControlActive = false; // Can't activate without an image
            return;
        }
        resetSamSelection();
        warningMessage = "Mode Controle SAM active";
        warningEndTime = ofGetElapsedTimef() + 2.0f;
    } else {
        warningMessage = "Mode Controle SAM desactive";
        warningEndTime = ofGetElapsedTimef() + 2.0f;
    }
}

void AnnexeApp::dragEvent(ofDragInfo dragInfo) {
    if (dragInfo.files.size() > 0) {
        string file = dragInfo.files[0];
        ofImage checkImg;
        if (checkImg.load(file)) {
            float imgRatio = checkImg.getWidth() / (float)checkImg.getHeight();
            float targetRatio = targetWidth / targetHeight;
            
            // Tolérance de 5% sur le ratio
            if (abs(imgRatio - targetRatio) > 0.05f) {
                warningMessage = "ATTENTION : Fichier aux mauvaises proportions ! (Ratio image: " + ofToString(imgRatio, 2) + " vs cible: " + ofToString(targetRatio, 2) + ")";
                warningEndTime = ofGetElapsedTimef() + 4.0f; // Affiche pendant 4 secondes
                ofLogWarning("AnnexeApp") << warningMessage;
            }
            
            img = checkImg;
            setupRipple();
            ofLogNotice("AnnexeApp") << "Image chargée dans l'annexe : " << file;
        }
    }
}

ofVec2f AnnexeApp::getTransformedMouse() {
    float mx = (ofGetMouseX() - viewPan.x) / viewZoom;
    float my = (ofGetMouseY() - viewPan.y) / viewZoom;
    return ofVec2f(mx, my);
}

void AnnexeApp::mouseScrolled(int x, int y, float scrollX, float scrollY) {
    if (scrollY == 0) return;
    ofVec2f worldM = getTransformedMouse();
    float zoomFactor = (scrollY > 0) ? 1.1f : 0.9f;
    viewZoom = ofClamp(viewZoom * zoomFactor, 0.01f, 10.0f);
    viewPan.x = x - worldM.x * viewZoom;
    viewPan.y = y - worldM.y * viewZoom;
}

void AnnexeApp::mousePressed(int x, int y, int button) {
    lastMouse.set(x, y); // Toujours mettre a jour pour eviter les sauts de vue au drag

    if (bSamControlActive) {
        if (samSaveBtn.inside(x, y)) {
            saveSamSegmentation();
            return;
        }
        if (samResetBtn.inside(x, y)) {
            resetSamSelection();
            return;
        }
    }

    if (bSamControlActive && !isSpacePressed) {
        ofVec2f worldM = getTransformedMouse();
        if (worldM.x >= 0 && worldM.x < targetWidth && worldM.y >= 0 && worldM.y < targetHeight) {
            samMousePressTime = ofGetElapsedTimef();
            samDragStart = worldM;
            bSamIsDragging = true;
        }
        return; // Consume mouse press
    }

    if (bRippleEffect && img.isAllocated() && !isSpacePressed) { // Ajout de la condition !isSpacePressed
        ofVec2f worldM = getTransformedMouse();
        // Check if click is inside the image area
        if (worldM.x >= 0 && worldM.x < targetWidth && worldM.y >= 0 && worldM.y < targetHeight) {
            int imgX = ofMap(worldM.x, 0, targetWidth, 0, img.getWidth());
            int imgY = ofMap(worldM.y, 0, targetHeight, 0, img.getHeight());
            createRippleAt(imgX, imgY);
        }
    }
}

void AnnexeApp::mouseDragged(int x, int y, int button) {
    if (bSamControlActive && bSamIsDragging) {
        // This is just for visual feedback, logic is in mouseReleased
        lastMouse.set(x, y);
        return;
    }
    if (isSpacePressed || !ofGetKeyPressed(OF_KEY_SHIFT)) {
        ofVec2f currentMouse(x, y);
        viewPan += (currentMouse - lastMouse);
    }
    lastMouse.set(x, y);
}
void AnnexeApp::mouseReleased(int x, int y, int button) {
    if (bSamControlActive && bSamIsDragging) {
        bSamIsDragging = false;
        ofVec2f worldM = getTransformedMouse();

        float dragDist = samDragStart.distance(worldM);
        float pressDuration = ofGetElapsedTimef() - samMousePressTime;

        int imgX = ofMap(worldM.x, 0, targetWidth, 0, img.getWidth());
        int imgY = ofMap(worldM.y, 0, targetHeight, 0, img.getHeight());
        
        int startImgX = ofMap(samDragStart.x, 0, targetWidth, 0, img.getWidth());
        int startImgY = ofMap(samDragStart.y, 0, targetHeight, 0, img.getHeight());

        if (dragDist < 10) { // It's a click
            if (!ofGetKeyPressed(OF_KEY_TAB)) { // Short click -> foreground
                samPoints.push_back(ofVec2f(imgX, imgY));
                samLabels.push_back(1);
            } else { // Long press -> background
                samPoints.push_back(ofVec2f(imgX, imgY));
                samLabels.push_back(0);
            }
        } else { // It's a drag -> bounding box
            // On ne garde qu'une seule boite de selection pour la clarte.
            // On supprime l'ancienne boite avant d'en creer une nouvelle.
            for (int i = samLabels.size() - 1; i >= 0; --i) {
                if (samLabels[i] == 2 || samLabels[i] == 3) {
                    samLabels.erase(samLabels.begin() + i);
                    samPoints.erase(samPoints.begin() + i);
                }
            }

            float x1 = std::min(startImgX, imgX);
            float y1 = std::min(startImgY, imgY);
            float x2 = std::max(startImgX, imgX);
            float y2 = std::max(startImgY, imgY);
            samPoints.push_back(ofVec2f(x1, y1));
            samLabels.push_back(2);
            samPoints.push_back(ofVec2f(x2, y2));
            samLabels.push_back(3);
        }
        runSamInference();
    }
}

void AnnexeApp::keyPressed(int key) {
    if (key == ' ') isSpacePressed = true;
    if (key == 'r' || key == 'R') setup(); // Reset vue
}

void AnnexeApp::keyReleased(int key) {
    if (key == ' ') isSpacePressed = false;
}

void AnnexeApp::setupRipple() {
    if (!img.isAllocated()) {
        rippleOutputImage.clear();
        rippleBuffer1.clear();
        rippleBuffer2.clear();
        rippleCols = 0;
        rippleRows = 0;
        return;
    }

    int w = img.getWidth();
    int h = img.getHeight();

    rippleOutputImage.allocate(w, h, OF_IMAGE_COLOR);

    rippleCols = (int)(w * rippleBufferScale);
    rippleRows = (int)(h * rippleBufferScale);

    rippleBuffer1.assign(rippleCols * rippleRows, 0);
    rippleBuffer2.assign(rippleCols * rippleRows, 0);
    
    ofLogNotice("AnnexeApp") << "Ripple effect setup for image " << w << "x" << h;
}

void AnnexeApp::updateRipple() {
    if (!img.isAllocated() || rippleCols == 0) return;
    processRipples();
    renderRipples();
    std::swap(rippleBuffer1, rippleBuffer2);
}

void AnnexeApp::createRippleAt(int localX, int localY) {
    if (rippleCols == 0) return;
    int bx = (int)(localX * rippleBufferScale);
    int by = (int)(localY * rippleBufferScale);

    for (int j = by - rippleSize; j < by + rippleSize; j++) {
        for (int k = bx - rippleSize; k < bx + rippleSize; k++) {
            if (j >= 1 && j < rippleRows - 1 && k >= 1 && k < rippleCols - 1) {
                rippleBuffer1[k + j * rippleCols] = 255;
            }
        }
    }
}

void AnnexeApp::processRipples() {
    for (int y = 1; y < rippleRows - 1; y++) {
        for (int x = 1; x < rippleCols - 1; x++) {
            int index = x + y * rippleCols;
            
            int val = (rippleBuffer1[index - 1] +
                       rippleBuffer1[index + 1] +
                       rippleBuffer1[index - rippleCols] +
                       rippleBuffer1[index + rippleCols]) >> 1;
            
            val -= rippleBuffer2[index];
            val -= val >> 5; // Damping
            
            rippleBuffer2[index] = val;
        }
    }
}

void AnnexeApp::renderRipples() {
    const unsigned char* srcPixels = img.getPixels().getData();
    unsigned char* dstPixels = rippleOutputImage.getPixels().getData();
    
    int iw = img.getWidth();
    int ih = img.getHeight();
    int channels = img.getPixels().getNumChannels();
    if (channels < 3) return;

    for (int y = 0; y < ih; y++) {
        for (int x = 0; x < iw; x++) {
            
            int xBuffer = (int)(x * rippleBufferScale);
            int yBuffer = (int)(y * rippleBufferScale);
            
            int pixelIndex = (x + y * iw) * channels;

            if (xBuffer > 0 && xBuffer < rippleCols - 1 && yBuffer > 0 && yBuffer < rippleRows - 1) {
                
                int index = xBuffer + yBuffer * rippleCols;
                
                int xOffset = rippleBuffer1[index - 1] - rippleBuffer1[index + 1];
                int yOffset = rippleBuffer1[index - rippleCols] - rippleBuffer1[index + rippleCols];
                
                int xCoord = x + xOffset;
                int yCoord = y + yOffset;
                
                xCoord = ofClamp(xCoord, 0, iw - 1);
                yCoord = ofClamp(yCoord, 0, ih - 1);
                
                int sourceIndex = (xCoord + yCoord * iw) * channels;
                
                for(int c=0; c<channels; ++c) {
                    dstPixels[pixelIndex + c] = srcPixels[sourceIndex + c];
                }
            } else {
                for(int c=0; c<channels; ++c) {
                    dstPixels[pixelIndex + c] = srcPixels[pixelIndex + c];
                }
            }
        }
    }
    rippleOutputImage.update();
}

void AnnexeApp::runSamInference() {
    if (!img.isAllocated() || samPoints.empty()) {
        samPreviewMask.clear();
        bSamMaskGenerated = false;
        return;
    }

    string modelPath = ofToDataPath("models/SAM/image_segmentation_efficientsam_ti_2025april_int8.onnx");
    if (!ofFile(modelPath).exists()) {
        ofLogWarning("AnnexeApp") << "Modele IA introuvable : " << modelPath;
        warningMessage = "Modele SAM introuvable !";
        warningEndTime = ofGetElapsedTimef() + 3.0f;
        return;
    }

    try {
        cv::dnn::Net net = cv::dnn::readNetFromONNX(modelPath);
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_DEFAULT);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

        int w = img.getWidth();
        int h = img.getHeight();

        ofPixels rgbPixels = img.getPixels();
        rgbPixels.setImageType(OF_IMAGE_COLOR);
        
        ofxCvColorImage colorImg;
        colorImg.allocate(w, h);
        colorImg.setFromPixels(rgbPixels);
        cv::Mat cvImg = cv::cvarrToMat(colorImg.getCvImage());

        cv::Mat image_blob = cv::dnn::blobFromImage(cvImg, 1.0 / 255.0, cv::Size(1024, 1024), cv::Scalar(0, 0, 0), false, false);
        
        const int max_points = 6;
        int shape_pts[4] = {1, 1, max_points, 2};
        cv::Mat points_blob(4, shape_pts, CV_32F, cv::Scalar(0));
        
        int shape_lbls[4] = {1, 1, max_points, 1};
        cv::Mat labels_blob(4, shape_lbls, CV_32F, cv::Scalar(-1));
        
        float* points_ptr = points_blob.ptr<float>();
        float* labels_ptr = labels_blob.ptr<float>();

        vector<ofVec2f> backgroundPoints;
        int fg_point_idx = 0;
        for (size_t i = 0; i < samPoints.size(); ++i) {
            if (samLabels[i] == 0) { // Background point
                backgroundPoints.push_back(samPoints[i]);
            } else { // Foreground or box point
                if (fg_point_idx < max_points) {
                    float scaled_x = samPoints[i].x * 1024.0f / w;
                    float scaled_y = samPoints[i].y * 1024.0f / h;
                    points_ptr[fg_point_idx * 2 + 0] = scaled_x;
                    points_ptr[fg_point_idx * 2 + 1] = scaled_y;
                    labels_ptr[fg_point_idx] = (float)samLabels[i];
                    fg_point_idx++;
                }
            }
        }

        if (fg_point_idx == 0) {
            samPreviewMask.clear();
            bSamMaskGenerated = false;
            return;
        }

        net.setInput(image_blob, "batched_images");
        net.setInput(points_blob, "batched_point_coords");
        net.setInput(labels_blob, "batched_point_labels");

        std::vector<cv::String> outNames = {"output_masks", "iou_predictions"};
        std::vector<cv::Mat> outputs;
        net.forward(outputs, outNames);

        if(outputs.size() < 2 || outputs[0].empty() || outputs[1].empty()) {
            ofLogError("AnnexeApp") << "Erreur: Le modele SAM n'a pas retourne les masques et IOUs attendus.";
            return;
        }

        cv::Mat outputBlob = outputs[0];
        cv::Mat outputIou = outputs[1];

        vector<pair<float, int>> sorted_ious;
        const float* iou_ptr = outputIou.ptr<float>();
        for(int i = 0; i < outputIou.total(); ++i) {
            sorted_ious.push_back({iou_ptr[i], i});
        }
        std::sort(sorted_ious.rbegin(), sorted_ious.rend());

        int outH = outputBlob.size[outputBlob.dims - 2];
        int outW = outputBlob.size[outputBlob.dims - 1];
        cv::Mat bestMask;

        for (const auto& iou_pair : sorted_ious) {
            int mask_idx = iou_pair.second;
            const float* mask_ptr = outputBlob.ptr<float>() + mask_idx * (outH * outW);
            cv::Mat maskMap(outH, outW, CV_32F, (void*)mask_ptr);
            
            cv::Mat mask8U;
            cv::threshold(maskMap, mask8U, 0.0, 255.0, cv::THRESH_BINARY);
            mask8U.convertTo(mask8U, CV_8U);
            cv::resize(mask8U, mask8U, cv::Size(w, h), 0, 0, cv::INTER_NEAREST);

            bool contains_bg = false;
            for (const auto& bg_pt : backgroundPoints) {
                if (mask8U.at<unsigned char>((int)bg_pt.y, (int)bg_pt.x) > 0) {
                    contains_bg = true;
                    break;
                }
            }

            if (!contains_bg) {
                bestMask = mask8U;
                break;
            }
        }

        if (bestMask.empty() && !sorted_ious.empty()) {
            int best_mask_idx = sorted_ious[0].second;
            const float* mask_ptr = outputBlob.ptr<float>() + best_mask_idx * (outH * outW);
            cv::Mat maskMap(outH, outW, CV_32F, (void*)mask_ptr);
            cv::threshold(maskMap, bestMask, 0.0, 255.0, cv::THRESH_BINARY);
            bestMask.convertTo(bestMask, CV_8U);
            cv::resize(bestMask, bestMask, cv::Size(w, h), 0, 0, cv::INTER_NEAREST);
        }

        if (!bestMask.empty()) {
            samPreviewMask.getPixels().setFromExternalPixels(bestMask.data, w, h, 1);
            samPreviewMask.update();
            bSamMaskGenerated = true;
        }

    } catch(const cv::Exception& e) {
        ofLogError("AnnexeApp") << "Erreur inference SAM DNN : " << e.what();
        warningMessage = "Erreur IA (SAM DNN)";
        warningEndTime = ofGetElapsedTimef() + 3.0f;
    }
}

void AnnexeApp::saveSamSegmentation() {
    if (!bSamMaskGenerated || !samPreviewMask.isAllocated()) {
        warningMessage = "Aucun masque a sauvegarder !";
        warningEndTime = ofGetElapsedTimef() + 3.0f;
        return;
    }

    ofDirectory dir("export");
    if(!dir.exists()) dir.create(true);
    
    string filename = "export/annexe_sam_interactive_" + ofGetTimestampString() + ".png";
    
    // Create a displayable RGBA image from the grayscale mask
    ofPixels finalPix;
    finalPix.allocate(samPreviewMask.getWidth(), samPreviewMask.getHeight(), OF_PIXELS_RGBA);
    for(int i=0; i<samPreviewMask.getPixels().size(); i++){
        unsigned char val = samPreviewMask.getPixels()[i];
        finalPix[i*4+0] = 255;
        finalPix[i*4+1] = 255;
        finalPix[i*4+2] = 255;
        finalPix[i*4+3] = val;
    }
    ofSaveImage(finalPix, filename);
    
    warningMessage = "Masque SAM sauvegarde : " + filename;
    warningEndTime = ofGetElapsedTimef() + 3.0f;
    ofLogNotice("AnnexeApp") << "Masque SAM interactif sauvegarde : " << filename;
}

void AnnexeApp::resetSamSelection() {
    samPoints.clear();
    samLabels.clear();
    samPreviewMask.clear();
    bSamMaskGenerated = false;
}