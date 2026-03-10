#include "GeminiImageGenerator.h"
#include <cctype>

// Fonction helper pour décoder le Base64 sans dépendre de Poco
static const std::string base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

static inline bool is_base64(unsigned char c) {
  return (std::isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_decode(std::string const& encoded_string) {
  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::string ret;

  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i ==4) {
      for (i = 0; i <4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++)
        ret += char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j <4; j++)
      char_array_4[j] = 0;

    for (j = 0; j <4; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
  }

  return ret;
}

// Fonction helper pour encoder en Base64
std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i) {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret += '=';
  }
  return ret;
}

//--------------------------------------------------------------
void GeminiImageGenerator::setup(string key) {
    apiKey = key;
    apiUrl = "https://generativelanguage.googleapis.com/v1beta/models/imagen-4.0-fast-generate-001:predict";
    api360Url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-3.1-flash-image-preview:generateContent";   
    nanoApiUrl = "https://generativelanguage.googleapis.com/v1beta/models/nano-banana-pro-preview:generateContent";
   //gemini-3.1-flash-image-preview
   //gemini-3-pro-image-preview
    
   //gemini-2.5-flash-image
   // models/nano-banana-pro-preview
    videoApiUrl = "https://generativelanguage.googleapis.com/v1beta/models/veo-3.1-fast-generate-preview:predictLongRunning";
   
   
    bIsLoading = false;
    bIsPolling = false;
    bNewImageAvailable = false;
    bNew360ImageAvailable = false;
    bIsRequest360 = false;
    bNewVideoAvailable = false;
    lastPollTime = 0;
    bRegistered = false;
    
    // On s'abonne aux notifications de réponse URL
    ofRegisterURLNotification(this);
    bRegistered = true;
    
    // On liste les modèles disponibles au démarrage pour déboguer
    listModels();
}

//--------------------------------------------------------------
void GeminiImageGenerator::generateImage(string prompt) {
    if(bIsLoading) {
        ofLogWarning("GeminiImageGenerator") << "Déjà en train de générer une image.";
        return;
    }

    ofLogNotice("GeminiImageGenerator") << "Envoi du prompt : " << prompt;
    bIsLoading = true;
    bNewImageAvailable = false;
    bIsRequest360 = false; // Ce n'est pas une requête 360

    // Construction du JSON pour l'API Imagen
    ofJson json;
    
    // Structure typique pour l'API REST Google (peut varier selon la version exacte de l'API utilisée)
    // Pour Imagen sur Vertex AI / Generative Language :
    json["instances"][0]["prompt"] = prompt;
    json["parameters"]["sampleCount"] = 1;
    json["parameters"]["aspectRatio"] = "1:1"; // ou "16:9", "9:16"
    json["parameters"]["outputOptions"]["mimeType"] = "image/jpeg";

    ofHttpRequest request;
    request.method = ofHttpRequest::POST;
    // Ajout de la clé API dans l'URL
    request.url = apiUrl + "?key=" + apiKey;
    request.headers["Content-Type"] = "application/json";
    request.body = json.dump();
    request.name = "GeminiImageGen"; // ID pour reconnaitre la requête

    loader.handleRequestAsync(request);
}

//--------------------------------------------------------------
void GeminiImageGenerator::generateImage360(string prompt) {
    if(bIsLoading) {
        ofLogWarning("GeminiImageGenerator") << "Déjà en train de générer.";
        return;
    }

    ofLogNotice("GeminiImageGenerator") << "Envoi du prompt 360 (Gemini 3.1) : " << prompt;
    bIsLoading = true;
    bNew360ImageAvailable = false;
    bIsRequest360 = true; // On active le flag 360

    ofJson json;
    // Structure pour Gemini 3.1 (generateContent)
    json["contents"][0]["parts"][0]["text"] = prompt + " , 360 view, equirectangular projection, vr, 8k, seamless";

    json["generationConfig"]["imageConfig"]["aspectRatio"] = "16:9"; // Format large
    //json["generationConfig"]["imageConfig"]["imageSize"] = "4K"; // Resolution 4K
    

    ofHttpRequest request;
    request.method = ofHttpRequest::POST;
    // Utilisation du modèle Gemini 3.1
    request.url = api360Url + "?key=" + apiKey;
    request.headers["Content-Type"] = "application/json";
    request.body = json.dump();
    request.name = "GeminiImageGen"; // On garde le même nom pour réutiliser le parsing JSON

    loader.handleRequestAsync(request);
}

//--------------------------------------------------------------
void GeminiImageGenerator::generateImage360FromImage(string prompt, string imagePath) {
    if(bIsLoading) {
        ofLogWarning("GeminiImageGenerator") << "Déjà en train de générer.";
        return;
    }

    // 1. Chargement de l'image depuis le disque
    ofFile file(imagePath);
    if(!file.exists()) {
        ofLogError("GeminiImageGenerator") << "Image introuvable : " << imagePath;
        return;
    }

    ofBuffer buffer = ofBufferFromFile(imagePath);
    if(buffer.size() == 0) {
        ofLogError("GeminiImageGenerator") << "Buffer vide pour : " << imagePath;
        return;
    }

    // 2. Encodage Base64
    string base64Img = base64_encode((unsigned char*)buffer.getData(), buffer.size());

    // 3. Détection basique du type MIME
    string mimeType = "image/png";
    string ext = ofToLower(file.getExtension());
    if (ext == "jpg" || ext == "jpeg") mimeType = "image/jpeg";

    ofLogNotice("GeminiImageGenerator") << "Envoi du prompt 360 Image-to-Image (Gemini 3.1) : " << prompt;
    bIsLoading = true;
    bNew360ImageAvailable = false;
    bIsRequest360 = true; // On active le flag 360 pour la sauvegarde

    ofJson json;
    // Structure Multimodale : Texte + Image Inline
    json["contents"][0]["parts"][0]["text"] = prompt + " , 360 view, equirectangular projection, vr, 8k, seamless";
    json["contents"][0]["parts"][1]["inline_data"]["mime_type"] = mimeType;
    json["contents"][0]["parts"][1]["inline_data"]["data"] = base64Img;
    //json["generationConfig"]["imageConfig"]["imageSize"] = "4K"; // Resolution 4K

    // Configuration
    // Note: On ne force pas l'aspectRatio ici pour laisser le modèle suivre l'image d'entrée (souvent 2:1 pour la 360)
    
    ofHttpRequest request;
    request.method = ofHttpRequest::POST;
    request.url = api360Url + "?key=" + apiKey; // Utilise gemini-3.1-flash-image-preview
    request.headers["Content-Type"] = "application/json";
    request.body = json.dump();
    request.name = "GeminiImageGen"; 

    loader.handleRequestAsync(request);
}

//--------------------------------------------------------------
void GeminiImageGenerator::generateNano360(string prompt) {
    if(bIsLoading) {
        ofLogWarning("GeminiImageGenerator") << "Déjà en train de générer.";
        return;
    }

    ofLogNotice("GeminiImageGenerator") << "Envoi du prompt 360 (Nano Banana) : " << prompt;
    bIsLoading = true;
    bNew360ImageAvailable = false;
    bIsRequest360 = true; // On active le flag 360 pour sauvegarde et affichage

    ofJson json;
    json["contents"][0]["parts"][0]["text"] = prompt + " , 360 view, equirectangular projection, vr, 8k, seamless";
    json["generationConfig"]["imageConfig"]["aspectRatio"] = "16:9"; 
    //json["generationConfig"]["imageConfig"]["imageSize"] = "4K"; // Resolution 4K

    ofHttpRequest request;
    request.method = ofHttpRequest::POST;
    request.url = nanoApiUrl + "?key=" + apiKey;
    request.headers["Content-Type"] = "application/json";
    request.body = json.dump();
    request.name = "GeminiImageGen"; 

    loader.handleRequestAsync(request);
}

//--------------------------------------------------------------
void GeminiImageGenerator::generateVideo(string prompt) {
    if(bIsLoading) {
        ofLogWarning("GeminiImageGenerator") << "Déjà en train de générer.";
        return;
    }

    ofLogNotice("GeminiImageGenerator") << "Envoi du prompt VIDEO (Veo) : " << prompt;
    bIsLoading = true;
    bNewVideoAvailable = false;
    bIsPolling = false;

    ofJson json;
    // Structure pour Veo (similaire à Imagen sur l'API Generative Language)
    json["instances"][0]["prompt"] = prompt;
    json["parameters"]["sampleCount"] = 1;
    json["parameters"]["aspectRatio"] = "16:9"; // Format vidéo
   // json["parameters"]["resolution"] = "4k"; // Format vidéo

    //json["parameters"]["outputOptions"]["mimeType"] = "video/mp4";

    ofHttpRequest request;
    request.method = ofHttpRequest::POST;
    // Endpoint pour Veo 2.0
    request.url = videoApiUrl + "?key=" + apiKey;
    request.headers["Content-Type"] = "application/json";
    request.body = json.dump();
    request.name = "GeminiVideoGen"; // ID spécifique pour la vidéo

    loader.handleRequestAsync(request);
}

void GeminiImageGenerator::generateVideoFromImage(string prompt, string imagePath) {
    if(bIsLoading) {
        ofLogWarning("GeminiImageGenerator") << "Déjà en train de générer.";
        return;
    }

    // 1. Chargement de l'image initiale depuis le disque
    ofFile file(imagePath);
    if(!file.exists()) {
        ofLogError("GeminiImageGenerator") << "Image introuvable : " << imagePath;
        return;
    }

    ofBuffer buffer = ofBufferFromFile(imagePath);
    if(buffer.size() == 0) {
        ofLogError("GeminiImageGenerator") << "Buffer vide pour : " << imagePath;
        return;
    }

    // 2. Encodage Base64
    string base64Img = base64_encode((unsigned char*)buffer.getData(), buffer.size());
    
    // 3. Détection basique du type MIME
    string mimeType = "image/png";
    string ext = ofToLower(file.getExtension());
    if (ext == "jpg" || ext == "jpeg") mimeType = "image/jpeg";

    ofLogNotice("GeminiImageGenerator") << "Envoi du prompt VIDEO (Veo Image-to-Video) : " << prompt;
    bIsLoading = true;
    bNewVideoAvailable = false;
    bIsPolling = false;

    ofJson json;
    // Structure pour Veo avec image de référence
    json["instances"][0]["prompt"] = prompt;
    json["instances"][0]["image"]["mimeType"] = mimeType;
    json["instances"][0]["image"]["bytesBase64Encoded"] = base64Img;
    
    json["parameters"]["sampleCount"] = 1;
    json["parameters"]["aspectRatio"] = "16:9"; // Ou "9:16" pour du portrait
    //json["parameters"]["resolution"] = "4k"; // Format vidéo
    
    ofHttpRequest request;
    request.method = ofHttpRequest::POST;
    request.url = videoApiUrl + "?key=" + apiKey;
    request.headers["Content-Type"] = "application/json";
    request.body = json.dump();
    
    // On réutilise le même nom de requête pour déclencher la même logique de Polling dans urlResponse
    request.name = "GeminiVideoGen"; 

    loader.handleRequestAsync(request);
}

void GeminiImageGenerator::generateVideoFromDeuxImages(string prompt, string imagePath1, string imagePath2) {
    if(bIsLoading) {
        ofLogWarning("GeminiImageGenerator") << "Déjà en train de générer.";
        return;
    }

    // 1. Chargement de l'image 1 (Start Frame)
    ofFile file1(imagePath1);
    if(!file1.exists()) {
        ofLogError("GeminiImageGenerator") << "Image 1 introuvable : " << imagePath1;
        return;
    }
    ofBuffer buffer1 = ofBufferFromFile(imagePath1);
    string base64Img1 = base64_encode((unsigned char*)buffer1.getData(), buffer1.size());
    string mimeType1 = "image/png";
    string ext1 = ofToLower(file1.getExtension());
    if (ext1 == "jpg" || ext1 == "jpeg") mimeType1 = "image/jpeg";

    // 2. Chargement de l'image 2 (Last Frame)
    ofFile file2(imagePath2);
    if(!file2.exists()) {
        ofLogError("GeminiImageGenerator") << "Image 2 introuvable : " << imagePath2;
        return;
    }
    ofBuffer buffer2 = ofBufferFromFile(imagePath2);
    string base64Img2 = base64_encode((unsigned char*)buffer2.getData(), buffer2.size());
    string mimeType2 = "image/png";
    string ext2 = ofToLower(file2.getExtension());
    if (ext2 == "jpg" || ext2 == "jpeg") mimeType2 = "image/jpeg";

    ofLogNotice("GeminiImageGenerator") << "Envoi du prompt VIDEO (Veo Image-to-Video Interpolation) : " << prompt;
    bIsLoading = true;
    bNewVideoAvailable = false;
    bIsPolling = false;

    ofJson json;
    // Structure pour Veo avec image de référence (Start Frame)
    json["instances"][0]["prompt"] = prompt;
    json["instances"][0]["image"]["mimeType"] = mimeType1;
    json["instances"][0]["image"]["bytesBase64Encoded"] = base64Img1;
    
    // Correction : lastFrame doit être au même niveau que image et prompt dans l'instance
    json["instances"][0]["lastFrame"]["mimeType"] = mimeType2;
    json["instances"][0]["lastFrame"]["bytesBase64Encoded"] = base64Img2;
    
    // Les paramètres globaux restent dans "parameters"
    json["parameters"]["sampleCount"] = 1;
    json["parameters"]["aspectRatio"] = "16:9"; 
    //json["parameters"]["resolution"] = "4k"; // Format vidéo

    ofHttpRequest request;
    request.method = ofHttpRequest::POST;
    request.url = videoApiUrl + "?key=" + apiKey;
    request.headers["Content-Type"] = "application/json";
    request.body = json.dump();
    
    // On réutilise le même nom de requête pour déclencher la même logique de Polling dans urlResponse
    request.name = "GeminiVideoGen"; 

    loader.handleRequestAsync(request);
}


//--------------------------------------------------------------
void GeminiImageGenerator::listModels() {
    ofLogNotice("GeminiImageGenerator") << "Demande de la liste des modèles...";
    ofHttpRequest request;
    // Utilisation de v1beta pour voir les modèles récents (Veo, Imagen 3, etc.)
    request.url = "https://generativelanguage.googleapis.com/v1beta/models?key=" + apiKey;
    request.name = "ListModels";
    loader.handleRequestAsync(request);
}

//--------------------------------------------------------------
void GeminiImageGenerator::update() {
    // Si on est en attente d'une vidéo (Polling)
    if(bIsPolling) {
        float now = ofGetElapsedTimef();
        // On vérifie toutes les 3 secondes
        if(now - lastPollTime > 3.0f) {
            pollOperation();
            lastPollTime = now;
        }
    }
}

//--------------------------------------------------------------
void GeminiImageGenerator::pollOperation() {
    ofHttpRequest request;
    request.url = "https://generativelanguage.googleapis.com/v1beta/" + operationName + "?key=" + apiKey;
    request.name = "GeminiVideoPoll"; // ID différent pour reconnaitre la réponse du polling
    loader.handleRequestAsync(request);
}

//--------------------------------------------------------------
ofImage& GeminiImageGenerator::getImage() {
    bNewImageAvailable = false;
    return generatedImage;
}

//--------------------------------------------------------------
void GeminiImageGenerator::clearImage() {
    generatedImage.clear();
}

//--------------------------------------------------------------
string GeminiImageGenerator::get360ImagePath() {
    bNew360ImageAvailable = false;
    return image360FilePath;
}

//--------------------------------------------------------------
string GeminiImageGenerator::getVideoPath() {
    bNewVideoAvailable = false;
    return videoFilePath;
}

//--------------------------------------------------------------
void GeminiImageGenerator::urlResponse(ofHttpResponse & response) {
    // Gestion de la liste des modèles
    if(response.request.name == "ListModels") {
        ofLogNotice("GeminiImageGenerator") << "--- MODELES DISPONIBLES (v1beta) ---";
        try {
            ofJson json = ofJson::parse(response.data);
            if(json.contains("models")) {
                for(auto& model : json["models"]) {
                    string name = model["name"];
                    ofLogNotice("GeminiImageGenerator") << " > " << name;
                    if(model.contains("supportedGenerationMethods")) {
                        ofLogNotice("GeminiImageGenerator") << "   Methods: " << model["supportedGenerationMethods"].dump();
                    }
                }
            } else {
                ofLogNotice("GeminiImageGenerator") << response.data;
            }
        } catch(std::exception& e) {
            ofLogNotice("GeminiImageGenerator") << "Raw Data: " << response.data;
        }
        ofLogNotice("GeminiImageGenerator") << "------------------------------------";
        return;
    }

    if(response.request.name != "GeminiImageGen" && response.request.name != "GeminiVideoGen" && response.request.name != "GeminiVideoPoll" && response.request.name != "GeminiVideoDownload") return;

    bIsLoading = false;

    if(response.status == 200 && response.request.name == "GeminiImageGen") {
        ofLogNotice("GeminiImageGenerator") << "Réponse reçue avec succès.";
        
        try {
            ofJson json = ofJson::parse(response.data);
            
            string base64Data = "";

            // CAS 1 : Format Imagen (predictions -> bytesBase64Encoded)
            if(json.contains("predictions") && !json["predictions"].empty()) {
                base64Data = json["predictions"][0]["bytesBase64Encoded"];
            }
            // CAS 2 : Format Gemini (candidates -> inlineData -> data)
            else if(json.contains("candidates") && !json["candidates"].empty()) {
                auto& parts = json["candidates"][0]["content"]["parts"];
                if(!parts.empty() && parts[0].contains("inlineData")) {
                    base64Data = parts[0]["inlineData"]["data"];
                }
            }

            if(!base64Data.empty()) {
                // Décodage Base64 manuel
                string decodedStr = base64_decode(base64Data);
                
                // Chargement dans ofImage via ofBuffer
                ofBuffer buffer(decodedStr.c_str(), decodedStr.size());
                
                if(bIsRequest360) {
                    ofImage tempImg;
                    if(tempImg.load(buffer)) {

                        // Sauvegarde pour la 360 full normal
                        image360FilePath = "gen360_" + ofGetTimestampString() + ".jpg";
                        tempImg.save(image360FilePath, OF_IMAGE_QUALITY_BEST);

                         // On sauvegarde l'ancienne image pour l'interpolation (avant d'écraser gen360_last.jpg)
                        ofFile oldFile("gen360_last.jpg");
                        if(oldFile.exists()){
                            oldFile.copyTo("gen360.jpg", true, true);
                            ofLogNotice("GeminiImageGenerator") << "Ancienne image 'gen360_last.jpg' copiée vers 'gen360.jpg'";
                        }
                        
                        // Redimensionnement de l'image à la taille demandée
                        tempImg.resize(1376, 768);
                        //ofLogNotice("GeminiImageGenerator") << "Image 360 redimensionnée en 1376x768.";
                        image360FilePath = "gen360_last.jpg";
                        tempImg.save(image360FilePath, OF_IMAGE_QUALITY_HIGH);
                
                        
                        
                        bNew360ImageAvailable = true;

                       
                        ofLogNotice("GeminiImageGenerator") << "Image 360 sauvegardée : " << image360FilePath;
                    } else {
                        ofLogError("GeminiImageGenerator") << "Echec du chargement du buffer 360.";
                    }
                } else if(generatedImage.load(buffer)) {
                  
                    string fileName = "gen_" + ofGetTimestampString() + ".jpg";
                    generatedImage.save(fileName, OF_IMAGE_QUALITY_BEST);
                    ofLogNotice("GeminiImageGenerator") << "Image standard sauvegardée : " << fileName;
                    bNewImageAvailable = true;
                    ofLogNotice("GeminiImageGenerator") << "Image décodée et chargée.";
                } else {
                    ofLogError("GeminiImageGenerator") << "Echec du chargement du buffer dans ofImage.";
                }
            } else {
                ofLogError("GeminiImageGenerator") << "JSON invalide ou pas d'image : " << response.data;
            }
            
        } catch (std::exception& e) {
            ofLogError("GeminiImageGenerator") << "Erreur parsing JSON: " << e.what();
        }
        
    } else if(response.status == 200 && response.request.name == "GeminiVideoGen") {
        ofLogNotice("GeminiImageGenerator") << "Job Vidéo lancé. Réception de l'opération...";
        
        try {
            ofJson json = ofJson::parse(response.data);
            // On récupère le nom de l'opération (ex: "operations/12345...")
            if(json.contains("name")) {
                operationName = json["name"];
                bIsPolling = true; // On active le polling
                bIsLoading = true; // On reste en état de chargement global
                lastPollTime = ofGetElapsedTimef();
                ofLogNotice("GeminiImageGenerator") << "Opération reçue : " << operationName << ". Début du polling...";
            } else {
                ofLogError("GeminiImageGenerator") << "Pas de nom d'opération dans la réponse : " << response.data;
            }
        } catch (std::exception& e) {
            ofLogError("GeminiImageGenerator") << "Erreur parsing JSON Video Start: " << e.what();
        }
    } else if(response.status == 200 && response.request.name == "GeminiVideoPoll") {
        // Réponse du polling
        try {
            ofJson json = ofJson::parse(response.data);
            
            // Est-ce que c'est fini ?
            if(json.contains("done") && json["done"] == true) {
                ofLogNotice("GeminiImageGenerator") << "Génération terminée ! Récupération du résultat...";
                bIsPolling = false; // Stop polling
                
                // CAS 1 : Veo 2.0 (URI de téléchargement)
                if(json.contains("response") && json["response"].contains("generateVideoResponse")) {
                    try {
                        string videoUri = json["response"]["generateVideoResponse"]["generatedSamples"][0]["video"]["uri"];
                        ofLogNotice("GeminiImageGenerator") << "URI Vidéo trouvée. Téléchargement : " << videoUri;
                        
                        // On lance le téléchargement du fichier (nécessite la clé API)
                        ofHttpRequest request;
                        request.url = videoUri + "&key=" + apiKey;
                        request.name = "GeminiVideoDownload";
                        loader.handleRequestAsync(request);
                        
                        // On laisse bIsLoading à true car on attend le téléchargement
                        return; 
                    } catch(std::exception& e) {
                        ofLogError("GeminiImageGenerator") << "Erreur extraction URI: " << e.what();
                        bIsLoading = false;
                    }
                }
                // CAS 2 : Base64 (Ancienne méthode ou autres modèles comme Imagen)
                else if(json.contains("response") && json["response"].contains("predictions")) {
                    bIsLoading = false;
                    string base64Data = json["response"]["predictions"][0]["bytesBase64Encoded"];
                    string decodedStr = base64_decode(base64Data);
                    
                    videoFilePath = "generated_video_" + ofGetTimestampString() + ".mp4";
                    ofBuffer buffer(decodedStr.c_str(), decodedStr.size());
                    ofBufferToFile(videoFilePath, buffer);
                    
                    bNewVideoAvailable = true;
                    ofLogNotice("GeminiImageGenerator") << "Vidéo sauvegardée : " << videoFilePath;
                } else {
                    bIsLoading = false;
                    ofLogError("GeminiImageGenerator") << "Opération terminée mais pas de vidéo trouvée dans le JSON.";
                    ofLogNotice("GeminiImageGenerator") << json.dump();
                }
            } else {
                // Pas encore fini
                ofLogVerbose("GeminiImageGenerator") << "En cours de traitement...";
                bIsLoading = true; // On reste en loading
                // On ne fait rien, le update() relancera un poll dans 3 secondes
            }
        } catch (std::exception& e) {
            ofLogError("GeminiImageGenerator") << "Erreur parsing JSON Video Poll: " << e.what();
            bIsPolling = false;
            bIsLoading = false;
        }
    } else if(response.status == 200 && response.request.name == "GeminiVideoDownload") {
        // Réception du fichier vidéo téléchargé
        ofLogNotice("GeminiImageGenerator") << "Vidéo téléchargée avec succès (" << response.data.size() << " bytes).";
        
        videoFilePath = "generated_video_" + ofGetTimestampString() + ".mp4";
        // response.data est un ofBuffer contenant le binaire du MP4
        ofBufferToFile(videoFilePath, response.data);
        
        bNewVideoAvailable = true;
        bIsLoading = false;
        
    } else {
        ofLogError("GeminiImageGenerator") << "Erreur API: " << response.status << " " << response.error << "\n" << response.data;
        bIsPolling = false;
        bIsLoading = false;
    }
}