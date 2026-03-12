#include "monitor.h"
#include <gdiplus.h>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <mmsystem.h> 
#include <mutex> 

// --- GLOBALE VARIABLEN ---
HWND hMonitorWindow = NULL;
std::vector<double> g_monitorWave;
std::vector<uint16_t> g_monitorHex;
std::mutex dataMutex; 

// --- LIVE AUDIO VARIABLEN ---
HWAVEIN hWaveIn = NULL;
const int NUM_BUFFERS = 4;
const int BUFFER_SIZE = 4800; 
WAVEHDR waveHeader[NUM_BUFFERS];
int16_t audioBuffers[NUM_BUFFERS][BUFFER_SIZE];
bool isLiveRecording = false;
Gdiplus::RectF btnRect;

void CALLBACK waveInProc(HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
    if (uMsg == WIM_DATA) {
        WAVEHDR* pHeader = (WAVEHDR*)dwParam1;
        if (pHeader->dwBytesRecorded > 0) {
            size_t numSamples = pHeader->dwBytesRecorded / 2;
            int16_t* rawData = (int16_t*)pHeader->lpData;

            std::vector<double> mono_audio(numSamples);
            double mean = 0;
            for (size_t i = 0; i < numSamples; i++) {
                mono_audio[i] = rawData[i];
                mean += rawData[i];
            }
            mean /= numSamples;
            for (auto& s : mono_audio) s -= mean;

            std::vector<uint16_t> live_hex;
            size_t chunk_size = mono_audio.size() / 11;
            double global_max = 0.0001;
            for (double val : mono_audio) if (std::abs(val) > global_max) global_max = std::abs(val);

            for (int i = 0; i < 11; i++) {
                double local_max = 0;
                size_t start = i * chunk_size;
                size_t end = (i == 10) ? mono_audio.size() : (i + 1) * chunk_size;
                for (size_t j = start; j < end; j++) {
                    if (std::abs(mono_audio[j]) > local_max) local_max = std::abs(mono_audio[j]);
                }
                live_hex.push_back(0x3C00 + (uint16_t)((local_max / global_max) * 0xFF));
            }

            {
                std::lock_guard<std::mutex> lock(dataMutex);
                g_monitorWave = mono_audio;
                g_monitorHex = live_hex;
            }
            
            if (hMonitorWindow) InvalidateRect(hMonitorWindow, NULL, FALSE);
        }

        if (isLiveRecording) {
            waveInAddBuffer(hwi, pHeader, sizeof(WAVEHDR));
        }
    }
}

void StartLiveAudio() {
    if (isLiveRecording) return;
    
    WAVEFORMATEX wfx = {};
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 1; 
    wfx.nSamplesPerSec = 48000; 
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = wfx.nChannels * (wfx.wBitsPerSample / 8);
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

    if (waveInOpen(&hWaveIn, WAVE_MAPPER, &wfx, (DWORD_PTR)waveInProc, 0, CALLBACK_FUNCTION) != MMSYSERR_NOERROR) {
        MessageBoxW(hMonitorWindow, L"Could not open microphone!", L"Error", MB_OK);
        return;
    }

    for(int i=0; i < NUM_BUFFERS; i++) {
        waveHeader[i].lpData = (LPSTR)audioBuffers[i];
        waveHeader[i].dwBufferLength = sizeof(audioBuffers[i]);
        waveHeader[i].dwFlags = 0;
        waveInPrepareHeader(hWaveIn, &waveHeader[i], sizeof(WAVEHDR));
        waveInAddBuffer(hWaveIn, &waveHeader[i], sizeof(WAVEHDR));
    }

    isLiveRecording = true;
    waveInStart(hWaveIn);
}

void StopLiveAudio() {
    if (!isLiveRecording) return;
    isLiveRecording = false;
    waveInStop(hWaveIn);
    waveInReset(hWaveIn);
    for(int i=0; i < NUM_BUFFERS; i++) {
        waveInUnprepareHeader(hWaveIn, &waveHeader[i], sizeof(WAVEHDR));
    }
    waveInClose(hWaveIn);
    hWaveIn = NULL;
}

void RegisterMonitorClass(HINSTANCE hInst) {
    WNDCLASSEXW wcMon = {sizeof(WNDCLASSEXW)};
    wcMon.lpfnWndProc = MonitorWndProc; 
    wcMon.hInstance = hInst;
    wcMon.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
    wcMon.lpszClassName = L"MonitorClass";
    RegisterClassExW(&wcMon);
}

LRESULT CALLBACK MonitorWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_LBUTTONDOWN: {
            // Prüfen, ob unser "selbstgemalter" Button geklickt wurde
            int mouseX = LOWORD(lParam);
            int mouseY = HIWORD(lParam);
            if (mouseX >= btnRect.X && mouseX <= btnRect.X + btnRect.Width &&
                mouseY >= btnRect.Y && mouseY <= btnRect.Y + btnRect.Height) {
                if (isLiveRecording) StopLiveAudio();
                else StartLiveAudio();
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            RECT rect;
            GetClientRect(hwnd, &rect);
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;

            // --- DOUBLE BUFFERING (Verhindert Flimmern komplett) ---
            Gdiplus::Bitmap memBitmap(width, height, PixelFormat32bppARGB);
            Gdiplus::Graphics g(&memBitmap);
            g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

            // Hintergrund zeichnen
            Gdiplus::SolidBrush bgBrush(Gdiplus::Color(255, 10, 10, 15));
            g.FillRectangle(&bgBrush, 0, 0, width, height);

            Gdiplus::Pen gridPen(Gdiplus::Color(100, 0, 150, 0), 1.0f);
            g.DrawLine(&gridPen, 0.0f, height / 2.0f, (float)width, height / 2.0f);

            // Daten abholen
            std::vector<double> waveCopy;
            std::vector<uint16_t> hexCopy;
            {
                std::lock_guard<std::mutex> lock(dataMutex);
                waveCopy = g_monitorWave;
                hexCopy = g_monitorHex;
            }

            // Welle und Hex-Codes zeichnen
            if (!waveCopy.empty() && hexCopy.size() == 11) {
                Gdiplus::Pen wavePen(Gdiplus::Color(255, 0, 220, 255), 1.5f);
                double max_amp = 0.0001;
                for (double v : waveCopy) if (std::abs(v) > max_amp) max_amp = std::abs(v);

                Gdiplus::PointF lastPoint(0, height / 2.0f);

                // Perfektes Stretching auf Fensterbreite! Welle bricht nie wieder ab.
                for (int x = 0; x < width; x++) {
                    double progress = (double)x / (double)(width - 1);
                    size_t dataIdx = (size_t)(progress * (waveCopy.size() - 1));
                    if (dataIdx >= waveCopy.size()) dataIdx = waveCopy.size() - 1;
                    
                    double val = waveCopy[dataIdx] / max_amp;
                    float y = (height / 2.0f) - (float)(val * (height / 2.5f)); 
                    
                    Gdiplus::PointF currentPoint((float)x, y);
                    if (x > 0) g.DrawLine(&wavePen, lastPoint, currentPoint);
                    lastPoint = currentPoint;
                }

                Gdiplus::FontFamily fontFamily(L"Consolas");
                Gdiplus::Font font(&fontFamily, 14, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
                Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 255, 215, 0));

                float chunkWidth = (float)width / 11.0f;
                for (int i = 0; i < 11; i++) {
                    std::wstringstream wss;
                    wss << L"0x" << std::uppercase << std::hex << std::setfill(L'0') << std::setw(4) << hexCopy[i];
                    
                    Gdiplus::PointF textPos(i * chunkWidth + 10.0f, 40.0f); 
                    g.DrawString(wss.str().c_str(), -1, &font, textPos, &textBrush);
                    g.DrawLine(&gridPen, i * chunkWidth, 0.0f, i * chunkWidth, (float)height); 
                }
            } else {
                Gdiplus::FontFamily fontFamily(L"Consolas");
                Gdiplus::Font font(&fontFamily, 14, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
                Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 100, 100, 100));
                g.DrawString(L"Ready... Waiting for audio signal.", -1, &font, Gdiplus::PointF(20, height/2 - 20), &textBrush);
            }
            // --- DEN EIGENEN BUTTON ZEICHNEN ---
            btnRect = Gdiplus::RectF(width - 200, 10, 180, 30);
            Gdiplus::SolidBrush btnBrush(isLiveRecording ? Gdiplus::Color(255, 200, 50, 50) : Gdiplus::Color(255, 50, 150, 50));
            g.FillRectangle(&btnBrush, btnRect);

            Gdiplus::FontFamily fontFamily(L"Consolas");
            Gdiplus::Font btnFont(&fontFamily, 13, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
            Gdiplus::SolidBrush btnTextBrush(Gdiplus::Color(255, 255, 255, 255));
            const WCHAR* btnText = isLiveRecording ? L"⬛ LIVE STOPPEN" : L"🔴 LIVE MIC STARTEN";
            g.DrawString(btnText, -1, &btnFont, Gdiplus::PointF(btnRect.X + 15, btnRect.Y + 7), &btnTextBrush);
            Gdiplus::Graphics screenG(hdc);
            screenG.DrawImage(&memBitmap, 0, 0, width, height);

            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_CLOSE: {
            StopLiveAudio(); 
            ShowWindow(hwnd, SW_HIDE);
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}