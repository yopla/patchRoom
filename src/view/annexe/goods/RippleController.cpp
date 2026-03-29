#include "RippleController.h"

void RippleController::setup(const ofImage& img) {
    if (!img.isAllocated()) {
        outputImage.clear();
        buffer1.clear();
        buffer2.clear();
        cols = 0;
        rows = 0;
        return;
    }

    int w = img.getWidth();
    int h = img.getHeight();

    outputImage.allocate(w, h, OF_IMAGE_COLOR);

    cols = (int)(w * bufferScale);
    rows = (int)(h * bufferScale);

    buffer1.assign(cols * rows, 0);
    buffer2.assign(cols * rows, 0);
    
    ofLogNotice("RippleController") << "Ripple effect setup for image " << w << "x" << h;
}

void RippleController::update(const ofImage& img) {
    if (!bActive || !img.isAllocated() || cols == 0) return;
    processRipples();
    renderRipples(img);
    std::swap(buffer1, buffer2);
}

void RippleController::draw(const ofImage& img, float x, float y, float w, float h) {
    if (bActive && outputImage.isAllocated()) {
        outputImage.draw(x, y, w, h);
    } else if (img.isAllocated()) {
        img.draw(x, y, w, h);
    }
}

void RippleController::createRippleAt(int localX, int localY) {
    if (cols == 0) return;
    int bx = (int)(localX * bufferScale);
    int by = (int)(localY * bufferScale);

    for (int j = by - rippleSize; j < by + rippleSize; j++) {
        for (int k = bx - rippleSize; k < bx + rippleSize; k++) {
            if (j >= 1 && j < rows - 1 && k >= 1 && k < cols - 1) {
                buffer1[k + j * cols] = 255;
            }
        }
    }
}

void RippleController::processRipples() {
    for (int y = 1; y < rows - 1; y++) {
        for (int x = 1; x < cols - 1; x++) {
            int index = x + y * cols;
            int val = (buffer1[index - 1] + buffer1[index + 1] + buffer1[index - cols] + buffer1[index + cols]) >> 1;
            val -= buffer2[index];
            val -= val >> 5; // Damping
            buffer2[index] = val;
        }
    }
}

void RippleController::renderRipples(const ofImage& img) {
    const unsigned char* srcPixels = img.getPixels().getData();
    unsigned char* dstPixels = outputImage.getPixels().getData();
    
    int iw = img.getWidth();
    int ih = img.getHeight();
    int channels = img.getPixels().getNumChannels();
    if (channels < 3) return;

    for (int y = 0; y < ih; y++) {
        for (int x = 0; x < iw; x++) {
            int xBuffer = (int)(x * bufferScale);
            int yBuffer = (int)(y * bufferScale);
            int pixelIndex = (x + y * iw) * channels;

            if (xBuffer > 0 && xBuffer < cols - 1 && yBuffer > 0 && yBuffer < rows - 1) {
                int index = xBuffer + yBuffer * cols;
                int xOffset = buffer1[index - 1] - buffer1[index + 1];
                int yOffset = buffer1[index - cols] - buffer1[index + cols];
                
                int xCoord = ofClamp(x + xOffset, 0, iw - 1);
                int yCoord = ofClamp(y + yOffset, 0, ih - 1);
                int sourceIndex = (xCoord + yCoord * iw) * channels;
                
                for(int c=0; c<channels; ++c) dstPixels[pixelIndex + c] = srcPixels[sourceIndex + c];
            } else {
                for(int c=0; c<channels; ++c) dstPixels[pixelIndex + c] = srcPixels[pixelIndex + c];
            }
        }
    }
    outputImage.update();
}