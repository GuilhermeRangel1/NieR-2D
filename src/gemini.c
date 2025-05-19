#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "cJSON.h"
#include "gemini.h"

#define API_KEY "SUA_CHAVE"
#define MAX_RESPOSTA  1024

typedef struct {
    char *ptr;
    size_t len;
} StringBuf;

static void sbInit(StringBuf *s) {
    s->len = 0;
    s->ptr = malloc(1);
    s->ptr[0] = '\0';
}

static size_t sbWrite(void *data, size_t size, size_t nmemb, void *userp) {
    size_t add = size * nmemb;
    StringBuf *s = (StringBuf *)userp;
    char *tmp = realloc(s->ptr, s->len + add + 1);
    if (!tmp) return 0;
    s->ptr = tmp;
    memcpy(s->ptr + s->len, data, add);
    s->len += add;
    s->ptr[s->len] = '\0';
    return add;
}

char *respt(const char *prompt) {
    CURL *curl = curl_easy_init();
    if (!curl) {
        return strdup("Erro ao iniciar libcurl.");
    }

    cJSON *root = cJSON_CreateObject();
    cJSON *contents = cJSON_AddArrayToObject(root, "contents");
    cJSON *mensagem = cJSON_CreateObject();
    cJSON_AddItemToArray(contents, mensagem);

    cJSON_AddStringToObject(mensagem, "role", "user");
    cJSON *parts = cJSON_AddArrayToObject(mensagem, "parts");
    cJSON *partObj = cJSON_CreateObject();
    cJSON_AddItemToArray(parts, partObj);
    cJSON_AddStringToObject(partObj, "text", prompt);

    char *jsonReq = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    const char *url =
        "https://generativelanguage.googleapis.com/"
        "v1beta/models/gemini-1.5-flash-latest:generateContent?key=" API_KEY;

    struct curl_slist *hdrs = NULL;
    hdrs = curl_slist_append(hdrs, "Content-Type: application/json; charset=utf-8");

    StringBuf resp;
    sbInit(&resp);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hdrs);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonReq);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(jsonReq));
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, sbWrite);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    CURLcode cret = curl_easy_perform(curl);
    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

    free(jsonReq);
    curl_slist_free_all(hdrs);
    curl_easy_cleanup(curl);

    if (cret != CURLE_OK) {
        return strdup(curl_easy_strerror(cret));
    }
    if (httpCode != 200) {
        char msg[64];
        snprintf(msg, sizeof(msg), "HTTP %ld devolvido pela API.", httpCode);
        return strdup(msg);
    }

    cJSON *rootResp = cJSON_Parse(resp.ptr);
    free(resp.ptr);

    if (!rootResp) {
        return strdup("Falha ao parsear JSON de resposta.");
    }

    cJSON *cands = cJSON_GetObjectItemCaseSensitive(rootResp, "candidates");
    char *finalResp = NULL;

    if (cJSON_IsArray(cands) && cJSON_GetArraySize(cands) > 0) {
        cJSON *first = cJSON_GetArrayItem(cands, 0);
        cJSON *contentObj = cJSON_GetObjectItemCaseSensitive(first, "content");
        cJSON *partsArr = cJSON_GetObjectItemCaseSensitive(contentObj, "parts");
        cJSON *firstPart = (partsArr && cJSON_GetArraySize(partsArr) > 0)
                            ? cJSON_GetArrayItem(partsArr, 0)
                            : NULL;
        cJSON *txt = firstPart
                        ? cJSON_GetObjectItemCaseSensitive(firstPart, "text")
                        : NULL;

        if (cJSON_IsString(txt)) {
            finalResp = strdup(txt->valuestring);
        } else {
            finalResp = strdup("Campo \"text\" ausente ou inválido.");
        }
    } else {
        cJSON *err = cJSON_GetObjectItemCaseSensitive(rootResp, "error");
        cJSON *msg = err ? cJSON_GetObjectItemCaseSensitive(err, "message") : NULL;
        finalResp = strdup((msg && cJSON_IsString(msg)) ? msg->valuestring : "Erro desconhecido da API.");
    }

    cJSON_Delete(rootResp);
    return finalResp;
}
   
