#include "monitor.h"
#include <gdiplus.h>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <algorithm>

// Hier werden die Variablen ECHT im Speicher angelegt
HWND hMonitorWindow = NULL;
std::vector<double> g_monitorWave;
std::vector<uint16_t> g_monitorHex;

void RegisterMonitorClass(HINSTANCE hInst) {
    WNDCLASSEXW wcMon = {sizeof(WNDCLASSEXW)};
    wcMon.lpfnWndProc = MonitorWndProc; 
    wcMon.hInstance = hInst;
    wcMon.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2); // Dunkler Hintergrund
    wcMon.lpszClassName = L"MonitorClass";
    RegisterClassExW(&wcMon);
}

LRESULT CALLBACK MonitorWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            RECT rect;
            GetClientRect(hwnd, &rect);
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;

            // GDI+ aktivieren
            Gdiplus::Graphics g(hdc);
            g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

            // Hintergrund: Tiefes Schwarz
            Gdiplus::SolidBrush bgBrush(Gdiplus::Color(255, 15, 15, 20));
            g.FillRectangle(&bgBrush, 0, 0, width, height);

            // Mittellinie (Null-Linie) zeichnen
            Gdiplus::Pen gridPen(Gdiplus::Color(100, 0, 150, 0), 1.0f);
            g.DrawLine(&gridPen, 0, height / 2, width, height / 2);

            if (!g_monitorWave.empty() && g_monitorHex.size() == 11) {
                // 1. WELLE ZEICHNEN (Cyan)
                Gdiplus::Pen wavePen(Gdiplus::Color(255, 0, 220, 255), 1.5f);
                double max_amp = 0.0001;
                for (double v : g_monitorWave) if (std::abs(v) > max_amp) max_amp = std::abs(v);

                size_t step = std::max((size_t)1, g_monitorWave.size() / width); 
                Gdiplus::PointF lastPoint(0, height / 2);

                for (int x = 0; x < width; x++) {
                    size_t dataIdx = x * step;
                    if (dataIdx >= g_monitorWave.size()) break;
                    
                    double val = g_monitorWave[dataIdx] / max_amp;
                    float y = (height / 2.0f) - (float)(val * (height / 2.5f)); 
                    
                    Gdiplus::PointF currentPoint((float)x, y);
                    if (x > 0) g.DrawLine(&wavePen, lastPoint, currentPoint);
                    lastPoint = currentPoint;
                }

                // 2. HEX-CODES ZEICHNEN (Gold)
                Gdiplus::FontFamily fontFamily(L"Consolas");
                Gdiplus::Font font(&fontFamily, 14, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
                Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 255, 215, 0));

                float chunkWidth = (float)width / 11.0f;
                for (int i = 0; i < 11; i++) {
                    std::wstringstream wss;
                    wss << L"0x" << std::uppercase << std::hex << g_monitorHex[i];
                    
                    Gdiplus::PointF textPos(i * chunkWidth + 10.0f, 10.0f); 
                    g.DrawString(wss.str().c_str(), -1, &font, textPos, &textBrush);
                    
                    g.DrawLine(&gridPen, i * chunkWidth, 0.0f, i * chunkWidth, (float)height);
                }
            } else {
                Gdiplus::FontFamily fontFamily(L"Consolas");
                Gdiplus::Font font(&fontFamily, 14, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
                Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 100, 100, 100));
                g.DrawString(L"Warte auf Signal... Bitte Datei analysieren.", -1, &font, Gdiplus::PointF(20, height/2 - 20), &textBrush);
            }

            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_CLOSE: {
            ShowWindow(hwnd, SW_HIDE); // Nur verstecken, nicht das ganze Programm beenden!
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}