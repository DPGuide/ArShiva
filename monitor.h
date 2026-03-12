#ifndef MONITOR_H
#define MONITOR_H

#include <windows.h>
#include <vector>
#include <cstdint>

// Globale Variablen für den Datenaustausch (extern bedeutet: "Die existieren irgendwo")
extern HWND hMonitorWindow;
extern std::vector<double> g_monitorWave;
extern std::vector<uint16_t> g_monitorHex;

// Die Funktion, um das Fenster bei Windows anzumelden
void RegisterMonitorClass(HINSTANCE hInst);

// Die Fenster-Steuerung
LRESULT CALLBACK MonitorWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif