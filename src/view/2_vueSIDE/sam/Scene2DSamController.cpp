#include "Scene2DSamController.h"

void Scene2DSamController::setup() {
    int x = 10;
    int y = 40;
    int spacing = 30;
    saveBtn.set(x, y, 120, 25);
    resetBtn.set(x, y + spacing, 120, 25);
    pointModeBtn.set(x, y + spacing * 2, 120, 25);
    boxModeBtn.set(x, y + spacing * 3, 120, 25);
    toPhysicBtn.set(x, y + spacing * 4, 120, 25);
    toGearsBtn.set(x, y + spacing * 5, 120, 25);
    toSoftBtn.set(x, y + spacing * 6, 120, 25);
    toAliveBtn.set(x, y + spacing * 7, 120, 25);
}

void Scene2DSamController::toggle(const ofImage& overlayImg, string& warningMessage, float& warningEndTime) {
    bIsActive = !bIsActive;
    bDrawBoxMode = false;
    bDrawPointMode = false;
    if (bIsActive) {
        if (!overlayImg.isAllocated()) {
            warningMessage = "Chargez un overlay (drag&drop) d'abord !";
            warningEndTime = ofGetElapsedTimef() + 3.0f;
            bIsActive = false;
            return;
        }
        reset();
        warningMessage = "Mode Controle SAM active";
        warningEndTime = ofGetElapsedTimef() + 2.0f;
    } else {
        warningMessage = "Mode Controle SAM desactive";
        warningEndTime = ofGetElapsedTimef() + 2.0f;
    }
}

void Scene2DSamController::reset() {
    points.clear();
    labels.clear();
    previewMask.clear();
    bMaskGenerated = false;
}

void Scene2DSamController::drawPreview(float viewZoom, const ofImage& overlayImg, const ofVec2f& currentMouseImgSpace) {
    if (!bIsActive || !overlayImg.isAllocated()) return;
    
    if (bMaskGenerated && previewMask.isAllocated()) {
        ofPushStyle();
        ofSetColor(255, 0, 0, 165);
        previewMask.draw(0, 0);
        ofPopStyle();
    }

    for (size_t i = 0; i < points.size(); i++) {
        if (labels[i] != 0 && labels[i] != 1) continue;
        ofPushStyle();
        if (labels[i] == 1) ofSetColor(0, 255, 0);
        else ofSetColor(255, 0, 0);
        ofFill();
        ofDrawCircle(points[i].x, points[i].y, 5 / viewZoom);
        ofPopStyle();
    }
    
    for (size_t i = 0; i < points.size(); ++i) {
        if (labels[i] == 2 && i + 1 < points.size() && labels[i+1] == 3) {
            ofVec2f p1 = points[i];
            ofVec2f p2 = points[i+1];
            ofPushStyle();
            ofNoFill();
            ofSetColor(0, 0, 255);
            ofSetLineWidth(2 / viewZoom);
            ofDrawRectangle(p1.x, p1.y, p2.x - p1.x, p2.y - p1.y);
            ofPopStyle();
            i++;
        }
    }
    
    if (bIsDragging && bDrawBoxMode) {
        ofPushStyle();
        ofNoFill();
        ofSetColor(0, 0, 255, 150);
        ofSetLineWidth(1 / viewZoom);
        ofDrawRectangle(dragStart.x, dragStart.y, currentMouseImgSpace.x - dragStart.x, currentMouseImgSpace.y - dragStart.y);
        ofPopStyle();
    }
}

void Scene2DSamController::drawUI() {
    if (!bIsActive) return;

    ofPushStyle();
    
    string modeText = "SELECT A MODE TO START";
    if (bDrawBoxMode) modeText = "BOX MODE - Drag to draw a box";
    else if (bDrawPointMode) modeText = "POINT MODE - FG: Clic | BG: TAB+Clic";
    ofDrawBitmapStringHighlight(modeText, 10, 20, ofColor(0), ofColor(255));

    ofSetColor(50, 180, 50);
    ofFill(); ofDrawRectangle(saveBtn);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(saveBtn);
    ofSetColor(255);
    ofDrawBitmapString("SAVE MASK", saveBtn.x + 15, saveBtn.y + 17);

    ofSetColor(180, 50, 50);
    ofFill(); ofDrawRectangle(resetBtn);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(resetBtn);
    ofSetColor(255);
    ofDrawBitmapString("RESET", resetBtn.x + 35, resetBtn.y + 17);

    if (bDrawPointMode) ofSetColor(50, 180, 180); else ofSetColor(80, 120, 120);
    ofFill(); ofDrawRectangle(pointModeBtn);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(pointModeBtn);
    ofSetColor(255);
    ofDrawBitmapString("POINT MODE", pointModeBtn.x + 20, pointModeBtn.y + 17);

    if (bDrawBoxMode) ofSetColor(50, 180, 180); else ofSetColor(80, 120, 120);
    ofFill(); ofDrawRectangle(boxModeBtn);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(boxModeBtn);
    ofSetColor(255);
    ofDrawBitmapString("BOX MODE", boxModeBtn.x + 25, boxModeBtn.y + 17);

    ofSetColor(50, 100, 180);
    ofFill(); ofDrawRectangle(toPhysicBtn);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(toPhysicBtn);
    ofSetColor(255);
    ofDrawBitmapString("SAM > PHYSIC", toPhysicBtn.x + 10, toPhysicBtn.y + 17);

    ofSetColor(180, 100, 50);
    ofFill(); ofDrawRectangle(toGearsBtn);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(toGearsBtn);
    ofSetColor(255);
    ofDrawBitmapString("SAM > GEARS", toGearsBtn.x + 15, toGearsBtn.y + 17);

    ofSetColor(180, 50, 180);
    ofFill(); ofDrawRectangle(toSoftBtn);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(toSoftBtn);
    ofSetColor(255);
    ofDrawBitmapString("SAM > SOFT", toSoftBtn.x + 20, toSoftBtn.y + 17);

    ofSetColor(50, 180, 130);
    ofFill(); ofDrawRectangle(toAliveBtn);
    ofNoFill(); ofSetColor(255); ofDrawRectangle(toAliveBtn);
    ofSetColor(255);
    ofDrawBitmapString("SAM > ALIVE", toAliveBtn.x + 15, toAliveBtn.y + 17);

    ofPopStyle();
}

bool Scene2DSamController::mousePressed(int x, int y, ofVec2f worldM, const ofImage& overlayImg, Scene2DLayerManager& layerManager, string& warningMessage, float& warningEndTime) {
    if (!bIsActive) return false;

    if (saveBtn.inside(x, y)) { saveSegmentation(warningMessage, warningEndTime); return true; }
    if (resetBtn.inside(x, y)) { reset(); return true; }
    if (toPhysicBtn.inside(x, y)) { convertToPhysicsBody(layerManager, overlayImg, warningMessage, warningEndTime); return true; }
    if (pointModeBtn.inside(x, y)) {
        bDrawPointMode = !bDrawPointMode;
        if (bDrawPointMode) bDrawBoxMode = false;
        return true;
    }
    if (boxModeBtn.inside(x, y)) {
        bDrawBoxMode = !bDrawBoxMode;
        if (bDrawBoxMode) bDrawPointMode = false;
        return true;
    }
    if (toGearsBtn.inside(x, y)) { convertToGearsBody(layerManager, overlayImg, warningMessage, warningEndTime); return true; }
    if (toSoftBtn.inside(x, y)) { convertToSoftBody(layerManager, overlayImg, warningMessage, warningEndTime); return true; }
    if (toAliveBtn.inside(x, y)) { convertToAliveBody(layerManager, overlayImg, warningMessage, warningEndTime); return true; }

    if (!ofGetKeyPressed(' ')) {
        float imgX = worldM.x;
        float imgY = worldM.y + 912;
        if ((bDrawPointMode || bDrawBoxMode) && overlayImg.isAllocated() && imgX >= 0 && imgX < overlayImg.getWidth() && imgY >= 0 && imgY < overlayImg.getHeight()) {
            dragStart = ofVec2f(imgX, imgY);
            bIsDragging = true;
            return true;
        }
    }
    return false;
}

void Scene2DSamController::mouseReleased(ofVec2f worldM, const ofImage& overlayImg, string& warningMessage, float& warningEndTime) {
    if (!bIsActive || !bIsDragging) return;
    bIsDragging = false;

    float imgX = worldM.x;
    float imgY = worldM.y + 912;
    float dragDist = dragStart.distance(ofVec2f(imgX, imgY));
    bool hasMadeSelection = false;

    if (bDrawBoxMode) {
        if (dragDist > 10) { 
            float x1 = std::min(dragStart.x, imgX);
            float y1 = std::min(dragStart.y, imgY);
            float x2 = std::max(dragStart.x, imgX);
            float y2 = std::max(dragStart.y, imgY);
            points.push_back(ofVec2f(x1, y1));
            labels.push_back(2);
            points.push_back(ofVec2f(x2, y2));
            labels.push_back(3);
            hasMadeSelection = true;
        }
    } else if (bDrawPointMode) { 
        if (dragDist < 10) { 
            if (!ofGetKeyPressed(OF_KEY_TAB)) { 
                points.push_back(ofVec2f(imgX, imgY));
                labels.push_back(1);
            } else { 
                points.push_back(ofVec2f(imgX, imgY));
                labels.push_back(0);
            }
            hasMadeSelection = true;
        }
    }

    if (hasMadeSelection) runInference(overlayImg, warningMessage, warningEndTime);
}

void Scene2DSamController::runInference(const ofImage& overlayImg, string& warningMessage, float& warningEndTime) {
    if (!overlayImg.isAllocated() || points.empty()) {
        previewMask.clear();
        bMaskGenerated = false;
        return;
    }

    string modelPath = ofToDataPath("models/SAM/image_segmentation_efficientsam_ti_2025april_int8.onnx");
    if (!ofFile(modelPath).exists()) {
        warningMessage = "Modele SAM introuvable !";
        warningEndTime = ofGetElapsedTimef() + 3.0f;
        return;
    }

    try {
        cv::dnn::Net net = cv::dnn::readNetFromONNX(modelPath);
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_DEFAULT);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

        int w = overlayImg.getWidth();
        int h = overlayImg.getHeight();

        ofPixels rgbPixels = overlayImg.getPixels();
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
            if (labels[i] == 0) {
                backgroundPoints.push_back(points[i]);
            } else {
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

        if (fg_point_idx == 0) { previewMask.clear(); bMaskGenerated = false; return; }

        net.setInput(image_blob, "batched_images");
        net.setInput(points_blob, "batched_point_coords");
        net.setInput(labels_blob, "batched_point_labels");

        std::vector<cv::String> outNames = {"output_masks", "iou_predictions"};
        std::vector<cv::Mat> outputs;
        net.forward(outputs, outNames);

        if(outputs.size() < 2 || outputs[0].empty() || outputs[1].empty()) return;

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
                    contains_bg = true; break;
                }
            }
            if (!contains_bg) { bestMask = mask8U; break; }
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
            previewMask.getPixels().setFromExternalPixels(bestMask.data, w, h, 1);
            previewMask.update();
            bMaskGenerated = true;
        }

    } catch(const cv::Exception& e) {
        ofLogError("Scene2DSamController") << "Erreur inference SAM DNN : " << e.what();
        warningMessage = "Erreur IA (SAM DNN)";
        warningEndTime = ofGetElapsedTimef() + 3.0f;
    }
}

void Scene2DSamController::saveSegmentation(string& warningMessage, float& warningEndTime) {
    if (!bMaskGenerated || !previewMask.isAllocated()) {
        warningMessage = "Aucun masque a sauvegarder !";
        warningEndTime = ofGetElapsedTimef() + 3.0f;
        return;
    }
    ofDirectory dir("export");
    if(!dir.exists()) dir.create(true);
    string filename = "export/scene2d_sam_interactive_" + ofGetTimestampString() + ".png";
    
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
    warningEndTime = ofGetElapsedTimef() + 3.0f;
}

void Scene2DSamController::convertToPhysicsBody(Scene2DLayerManager& layerManager, const ofImage& overlayImg, string& warningMessage, float& warningEndTime) {
    if (!bMaskGenerated || !previewMask.isAllocated() || !overlayImg.isAllocated()) {
        warningMessage = "Aucun masque SAM ou overlay a convertir !";
        warningEndTime = ofGetElapsedTimef() + 3.0f;
        return;
    }

    layerManager.physicSamLayer.addBody(previewMask, overlayImg, -912.0f);
    layerManager.bDrawPhysicSam = true;
    reset();
    bIsActive = false; bDrawBoxMode = false; bDrawPointMode = false;
    warningMessage = "Corps physique cree. Mode SAM desactive.";
    warningEndTime = ofGetElapsedTimef() + 2.0f;
}

void Scene2DSamController::convertToGearsBody(Scene2DLayerManager& layerManager, const ofImage& overlayImg, string& warningMessage, float& warningEndTime) {
    if (!bMaskGenerated || !previewMask.isAllocated() || !overlayImg.isAllocated()) {
        warningMessage = "Aucun masque SAM ou overlay a convertir !";
        warningEndTime = ofGetElapsedTimef() + 3.0f;
        return;
    }

    layerManager.physicSamLayer.addGear(previewMask, overlayImg, -912.0f);
    layerManager.bDrawPhysicSam = true;
    reset();
    bIsActive = false; bDrawBoxMode = false; bDrawPointMode = false;
    warningMessage = "Engrenage cree. Mode SAM desactive.";
    warningEndTime = ofGetElapsedTimef() + 2.0f;
}

void Scene2DSamController::convertToSoftBody(Scene2DLayerManager& layerManager, const ofImage& overlayImg, string& warningMessage, float& warningEndTime) {
    if (!bMaskGenerated || !previewMask.isAllocated() || !overlayImg.isAllocated()) {
        warningMessage = "Aucun masque SAM ou overlay a convertir !";
        warningEndTime = ofGetElapsedTimef() + 3.0f;
        return;
    }
    layerManager.physicSamLayer.addSoftBody(previewMask, overlayImg, -912.0f);
    layerManager.bDrawPhysicSam = true;
    reset();
    bIsActive = false; bDrawBoxMode = false; bDrawPointMode = false;
    warningMessage = "Corps Elastique (Soft) cree. Mode SAM desactive.";
    warningEndTime = ofGetElapsedTimef() + 2.0f;
}

void Scene2DSamController::convertToAliveBody(Scene2DLayerManager& layerManager, const ofImage& overlayImg, string& warningMessage, float& warningEndTime) {
    if (!bMaskGenerated || !previewMask.isAllocated() || !overlayImg.isAllocated()) {
        warningMessage = "Aucun masque SAM ou overlay a convertir !";
        warningEndTime = ofGetElapsedTimef() + 3.0f;
        return;
    }
    layerManager.physicSamLayer.addAliveBody(previewMask, overlayImg, -912.0f);
    layerManager.bDrawPhysicSam = true;
    reset();
    bIsActive = false; bDrawBoxMode = false; bDrawPointMode = false;
    warningMessage = "Corps 'Alive' cree. Mode SAM desactive.";
    warningEndTime = ofGetElapsedTimef() + 2.0f;
}