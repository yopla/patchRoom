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

//--------------------------------------------------------------
void GeminiImageGenerator::setup(string key) {
    apiKey = key;
    // Endpoint pour Imagen via l'API Generative Language (Google AI Studio)
    // Note: Vérifie le modèle disponible pour ta clé (ex: imagen-3.0-generate-001)
    apiUrl = "https://generativelanguage.googleapis.com/v1beta/models/imagen-4.0-fast-generate-001:predict";
    
    // Endpoint pour Veo (Vidéo). Si "veo-2.0-generate-001" ne marche pas, vérifiez les logs de listModels()
    // Il est possible que le modèle s'appelle différemment ou ne soit pas encore public sur cette API.
    videoApiUrl = "https://generativelanguage.googleapis.com/v1beta/models/veo-3.0-fast-generate-001:predictLongRunning";
    
    bIsLoading = false;
    bIsPolling = false;
    bNewImageAvailable = false;
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

    // Construction du JSON pour l'API Imagen
    ofJson json;
    
    // Structure typique pour l'API REST Google (peut varier selon la version exacte de l'API utilisée)
    // Pour Imagen sur Vertex AI / Generative Language :
    json["instances"][0]["prompt"] = prompt;
    json["parameters"]["sampleCount"] = 1;
    json["parameters"]["aspectRatio"] = "1:1"; // ou "16:9", "9:16"
    json["parameters"]["outputOptions"]["mimeType"] = "image/png";

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
            
            // Le format de réponse contient généralement "predictions" -> "bytesBase64Encoded"
            if(json.contains("predictions") && !json["predictions"].empty()) {
                string base64Data = json["predictions"][0]["bytesBase64Encoded"];
                
                // Décodage Base64 manuel
                string decodedStr = base64_decode(base64Data);
                
                // Chargement dans ofImage via ofBuffer
                ofBuffer buffer(decodedStr.c_str(), decodedStr.size());
                if(generatedImage.load(buffer)) {
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
                    
                    videoFilePath = "generated_video.mp4";
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
        
        videoFilePath = "generated_video.mp4";
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