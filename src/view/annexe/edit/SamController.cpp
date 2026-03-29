#include "SamController.h"

void SamController::setup() {
    saveBtn.set(10, 60, 100, 25);
    resetBtn.set(10, 90, 100, 25);
}

void SamController::activate(const ofImage& img) {
    if (!img.isAllocated()) {
        warningMessage = "Chargez une image d'abord !";
        bIsActive = false;
        return;
    }
    bIsActive = true;
    reset();
    warningMessage = "Mode Controle SAM active";
}

void SamController::deactivate() {
    bIsActive = false;
    warningMessage = "Mode Controle SAM desactive";
}

void SamController::reset() {
    points.clear();
    labels.clear();
    previewMask.clear();
    bMaskGenerated = false;
}

string SamController::getWarningMessage() {
    return warningMessage;
}

void SamController::clearWarningMessage() {
    warningMessage = "";
}

void SamController::draw(float targetWidth, float targetHeight, float viewZoom, float imageWidth, float imageHeight) {
    if (!isActive() || imageWidth == 0 || imageHeight == 0) return;

    // Draw SAM mask overlay
    if (bMaskGenerated && previewMask.isAllocated()) {
        ofPushStyle();
        ofSetColor(255, 0, 0, 165); // Red overlay with ~65% opacity
        previewMask.draw(0, 0, targetWidth, targetHeight);
        ofPopStyle();
    }

    // Draw points
    for (size_t i = 0; i < points.size(); i++) {
        if (labels[i] != 0 && labels[i] != 1) { // box points, don't draw them individually
            continue;
        }

        ofVec2f p = points[i];
        // map point from image space to screen space (target space)
        float screenX = ofMap(p.x, 0, imageWidth, 0, targetWidth);
        float screenY = ofMap(p.y, 0, imageHeight, 0, targetHeight);

        ofPushStyle();
        if (labels[i] == 1) {
            ofSetColor(0, 255, 0);
        } else { // background (labels[i] == 0)
            ofSetColor(255, 0, 0);
        }
        ofFill();
        ofDrawCircle(screenX, screenY, 5 / viewZoom);
        ofPopStyle();
    }
    
    // Draw bounding boxes from points
    for (size_t i = 0; i < points.size(); ++i) {
        if (labels[i] == 2 && i + 1 < points.size() && labels[i+1] == 3) {
            ofVec2f p1 = points[i];
            ofVec2f p2 = points[i+1];
            float screenX1 = ofMap(p1.x, 0, imageWidth, 0, targetWidth);
            float screenY1 = ofMap(p1.y, 0, imageHeight, 0, targetHeight);
            float screenX2 = ofMap(p2.x, 0, imageWidth, 0, targetWidth);
            float screenY2 = ofMap(p2.y, 0, imageHeight, 0, targetHeight);
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

void SamController::drawDragFeedback(ofVec2f currentMouseInImageSpace, float targetWidth, float targetHeight, float imageWidth, float imageHeight, float viewZoom) {
    if (!isDragging() || imageWidth == 0 || imageHeight == 0) return;

    float startScreenX = ofMap(dragStartInImageSpace.x, 0, imageWidth, 0, targetWidth);
    float startScreenY = ofMap(dragStartInImageSpace.y, 0, imageHeight, 0, targetHeight);
    float currentScreenX = ofMap(currentMouseInImageSpace.x, 0, imageWidth, 0, targetWidth);
    float currentScreenY = ofMap(currentMouseInImageSpace.y, 0, imageHeight, 0, targetHeight);

    ofPushStyle();
    ofNoFill();
    ofSetColor(0, 0, 255, 150);
    ofSetLineWidth(1.0f / viewZoom);
    ofDrawRectangle(startScreenX, startScreenY, currentScreenX - startScreenX, currentScreenY - startScreenY);
    ofPopStyle();
}

void SamController::drawUI() {
    if (!isActive()) return;

    ofPushStyle();
    ofDrawBitmapStringHighlight("FG: Clic | BG: TAB+Clic | BOX: Drag", 120, 77, ofColor(0), ofColor(255));
    ofDrawBitmapStringHighlight("Sauver", saveBtn.x + 5, saveBtn.y + 17, ofColor(50, 180, 50), ofColor(255));
    ofDrawBitmapStringHighlight("Reset", resetBtn.x + 5, resetBtn.y + 17, ofColor(180, 50, 50), ofColor(255));
    ofPopStyle();
}

bool SamController::mousePressed(int x, int y, ofVec2f mouseInImageSpace) {
    if (!isActive()) return false;

    if (saveBtn.inside(x, y)) {
        saveSegmentation();
        return true;
    }
    if (resetBtn.inside(x, y)) {
        reset();
        return true;
    }

    dragStartInImageSpace = mouseInImageSpace;
    bIsDragging = true;
    return true; // Consume mouse press
}

void SamController::mouseDragged(ofVec2f mouseInImageSpace) {
    if (!isActive() || !bIsDragging) return;
    // The logic is in mouseReleased, this is just for visual feedback
}

void SamController::mouseReleased(ofVec2f mouseInImageSpace, const ofImage& img) {
    if (!isActive() || !bIsDragging) return;

    bIsDragging = false;

    float dragDist = dragStartInImageSpace.distance(mouseInImageSpace);

    // A 10px drag in screen space is a good threshold for a click. We approximate it in image space.
    float clickThresholdInImageSpace = 10.0f * (img.getWidth() / 2048.0f);

    if (dragDist < clickThresholdInImageSpace) { // It's a click
        if (!ofGetKeyPressed(OF_KEY_TAB)) { // Short click -> foreground
            points.push_back(mouseInImageSpace);
            labels.push_back(1);
        } else { // Long press -> background
            points.push_back(mouseInImageSpace);
            labels.push_back(0);
        }
    } else { // It's a drag -> bounding box
        float x1 = std::min(dragStartInImageSpace.x, mouseInImageSpace.x);
        float y1 = std::min(dragStartInImageSpace.y, mouseInImageSpace.y);
        float x2 = std::max(dragStartInImageSpace.x, mouseInImageSpace.x);
        float y2 = std::max(dragStartInImageSpace.y, mouseInImageSpace.y);
        points.push_back(ofVec2f(x1, y1));
        labels.push_back(2);
        points.push_back(ofVec2f(x2, y2));
        labels.push_back(3);
    }
    runInference(img);
}

void SamController::runInference(const ofImage& img) {
    if (!img.isAllocated() || points.empty()) {
        previewMask.clear();
        bMaskGenerated = false;
        return;
    }

    string modelPath = ofToDataPath("models/SAM/image_segmentation_efficientsam_ti_2025april_int8.onnx");
    if (!ofFile(modelPath).exists()) {
        ofLogWarning("SamController") << "Modele IA introuvable : " << modelPath;
        warningMessage = "Modele SAM introuvable !";
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
        for (size_t i = 0; i < points.size(); ++i) {
            if (labels[i] == 0) { // Background point
                backgroundPoints.push_back(points[i]);
            } else { // Foreground or box point
                if (fg_point_idx < max_points) {
                    float scaled_x = points[i].x * 1024.0f / w;
                    float scaled_y = points[i].y * 1024.0f / h;
                    points_ptr[fg_point_idx * 2 + 0] = scaled_x;
                    points_ptr[fg_point_idx * 2 + 1] = scaled_y;
                    labels_ptr[fg_point_idx] = (float)labels[i];
                    fg_point_idx++;
                }
            }
        }

        if (fg_point_idx == 0) {
            previewMask.clear();
            bMaskGenerated = false;
            return;
        }

        net.setInput(image_blob, "batched_images");
        net.setInput(points_blob, "batched_point_coords");
        net.setInput(labels_blob, "batched_point_labels");

        std::vector<cv::String> outNames = {"output_masks", "iou_predictions"};
        std::vector<cv::Mat> outputs;
        net.forward(outputs, outNames);

        if(outputs.size() < 2 || outputs[0].empty() || outputs[1].empty()) {
            ofLogError("SamController") << "Erreur: Le modele SAM n'a pas retourne les masques et IOUs attendus.";
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
            if (!previewMask.isAllocated() || previewMask.getWidth() != w || previewMask.getHeight() != h) {
                previewMask.allocate(w, h, OF_IMAGE_GRAYSCALE);
            }
            previewMask.getPixels().setFromExternalPixels(bestMask.data, w, h, 1);
            previewMask.update();
            bMaskGenerated = true;
        }

    } catch(const cv::Exception& e) {
        ofLogError("SamController") << "Erreur inference SAM DNN : " << e.what();
        warningMessage = "Erreur IA (SAM DNN)";
    }
}

void SamController::saveSegmentation() {
    if (!bMaskGenerated || !previewMask.isAllocated()) {
        warningMessage = "Aucun masque a sauvegarder !";
        return;
    }

    ofDirectory dir("export");
    if(!dir.exists()) dir.create(true);
    
    string filename = "export/annexe_sam_interactive_" + ofGetTimestampString() + ".png";
    
    ofPixels finalPix;
    finalPix.allocate(previewMask.getWidth(), previewMask.getHeight(), OF_PIXELS_RGBA);
    for(int i=0; i<previewMask.getPixels().size(); i++){
        unsigned char val = previewMask.getPixels()[i];
        finalPix[i*4+0] = 255;
        finalPix[i*4+1] = 255;
        finalPix[i*4+2] = 255;
        finalPix[i*4+3] = val;
    }
    ofSaveImage(finalPix, filename);
    
    warningMessage = "Masque SAM sauvegarde : " + filename;
    ofLogNotice("SamController") << "Masque SAM interactif sauvegarde : " << filename;
}