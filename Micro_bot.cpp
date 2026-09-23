#include <windows.h>
#include <sapi.h>
#include <iostream>
#include <string>

static const CLSID CLSID_Voz = {0x96749377,0x3391,0x11D2,{0x9E,0xE3,0x00,0xC0,0x4F,0x79,0x73,0x96}};
static const IID IID_Voz = {0x6C44DF74,0x72B9,0x4992,{0xA1,0xEC,0xEF,0x99,0x6E,0x04,0x22,0xD4}};

std::wstring paraLargo(const std::string& s) {
    if (s.empty()) return L"";
    int n = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0);
    std::wstring w(n, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, &w[0], n);
    if (!w.empty() && w.back() == L'\0') w.pop_back();
    return w;
}

std::string marcarPausas(const std::string& t) {
    std::string r;
    r.reserve(t.size() + 64);
    for (size_t i = 0; i < t.size(); i++) {
        char c = t[i];
        r += c;
        if (c == ',' || c == ';' || c == ':') r += " <silence msec=\"180\"/>";
        else if (c == '.') r += " <silence msec=\"350\"/>";
        else if (c == '!' || c == '?') r += " <silence msec=\"420\"/>";
        else if (c == '\n') r += " <silence msec=\"300\"/>";
    }
    return r;
}

int main(int argc, char* argv[]) {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    std::string texto;
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            texto += argv[i];
            if (i < argc - 1) texto += " ";
        }
    } else {
        std::cout << "Texto: ";
        std::getline(std::cin, texto);
    }

    if (texto.empty()) {
        std::cout << "Nada pra fala" << std::endl;
        return 0;
    }

    if (FAILED(CoInitialize(NULL))) return 1;

    ISpVoice* voz = NULL;
    HRESULT hr = CoCreateInstance(CLSID_Voz, NULL, CLSCTX_ALL, IID_Voz, (void**)&voz);
    if (FAILED(hr)) {
        std::cout << "Erro ao iniciar" << std::endl;
        CoUninitialize();
        return 1;
    }

    voz->SetRate(2);

    std::wstring pronto = paraLargo(marcarPausas(texto));
    voz->Speak(pronto.c_str(), SPF_ASYNC | SPF_IS_XML, NULL);
    voz->WaitUntilDone(INFINITE);

    voz->Release();
    CoUninitialize();
    return 0;
}