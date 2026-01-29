#ifdef _WIN32
    #undef main
    #define main WinMain
#endif

#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/listctrl.h>
#include <wx/textdlg.h>
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <wx/statline.h>
#include <wx/numdlg.h>
#include <wx/spinctrl.h> 
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "core/tasks/histogram.h"
#include "core/tasks/task_prioritizer.h"
#include "core/tasks/most_frequent_substrings.h"
#include "core/structures/DynamicArray.h"
#include "core/structures/Sequence.h"

// ID для элементов управления
enum ControlIDs {
    ID_HistogramGenerate = wxID_HIGHEST + 1,
    ID_HistogramClear,
    ID_AddIncident,
    ID_AddTask,
    ID_PrioritizeTasks,
    ID_ClearTasks,
    ID_FindSubstrings,
    ID_ClearSubstrings,
    ID_AnalyzeFile,
    ID_ClearFile
};

std::string WxStringToString(const wxString& str) {
    return str.ToStdString(wxConvUTF8);
}

wxString StringToWxString(const std::string& str) {
    return wxString::FromUTF8(str.c_str());
}

DynamicArray<char> StringToDynamicArray(const std::string& str) {
    DynamicArray<char> result(static_cast<int>(str.length()));
    for (size_t i = 0; i < str.length(); i++) {
        result.Set(static_cast<int>(i), str[i]);
    }
    return result;
}

std::string DynamicArrayToString(const DynamicArray<char>& arr) {
    std::string result;
    for (int i = 0; i < arr.GetSize(); i++) {
        result += arr.Get(i);
    }
    return result;
}

DynamicArray<DynamicArray<char>> StringVectorToDynamicArray(const std::vector<std::string>& vec) {
    DynamicArray<DynamicArray<char>> result(vec.size());
    for (size_t i = 0; i < vec.size(); i++) {
        DynamicArray<char> daStr = StringToDynamicArray(vec[i]);
        result.Set(static_cast<int>(i), daStr);
    }
    return result;
}

//  ГЛАВНОЕ ОКНО 

class MainWindow : public wxFrame {
private:
    wxNotebook* notebook;
    
    // Элементы для гистограммы
    wxTextCtrl* histInputData;
    wxTextCtrl* histNumRanges;
    wxListCtrl* histResults;
    wxStaticText* histStats;
    
    // Элементы для приоритизации задач
    wxListCtrl* incidentList;
    wxListCtrl* taskList;
    wxListCtrl* prioritizedList;
    
    // Элементы для поиска подстрок
    wxNotebook* substringsNotebook;
    
    // Вкладка "Анализ строки"
    wxTextCtrl* substringInput;
    wxTextCtrl* minLengthCtrl;
    wxTextCtrl* maxLengthCtrl;
    wxTextCtrl* topNControl;
    wxListCtrl* substringsResults;
    
    // Вкладка "Анализ файла"
    wxTextCtrl* fileContentText;
    wxListCtrl* fileResultsList;
    wxSpinCtrl* fileMinLength;
    wxSpinCtrl* fileMaxLength;
    wxSpinCtrl* fileTopN;
    wxStaticText* fileNameLabel;
    wxString currentFileName;
    std::string fileContent;
    
    // Объекты задач 
    Histogram<double> histogram;
    TaskPrioritizer taskPrioritizer;
    MostFrequentSubstrings substringFinder;
    
    std::vector<std::pair<std::vector<std::string>, int>> incidentData; 
    std::vector<std::pair<std::string, std::vector<std::string>>> taskData;
    
public:
    MainWindow(const wxString& title);
    
private:
    // Создание панелей
    wxPanel* CreateHistogramPanel(wxWindow* parent);
    wxPanel* CreateTaskPrioritizerPanel(wxWindow* parent);
    wxPanel* CreateSubstringsPanel(wxWindow* parent);
    
    // Создание вкладок для анализа подстрок
    wxPanel* CreateStringAnalysisPanel(wxWindow* parent);
    wxPanel* CreateFileAnalysisPanel(wxWindow* parent);
    
    // Обработчики событий
    void OnHistogramGenerate(wxCommandEvent& event);
    void OnHistogramClear(wxCommandEvent& event);
    
    void OnAddIncident(wxCommandEvent& event);
    void OnAddTask(wxCommandEvent& event);
    void OnPrioritizeTasks(wxCommandEvent& event);
    void OnClearTasks(wxCommandEvent& event);
    
    void OnFindSubstrings(wxCommandEvent& event);
    void OnClearSubstrings(wxCommandEvent& event);
    
    void OnAnalyzeFile(wxCommandEvent& event);
    void OnClearFile(wxCommandEvent& event);
    
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    
    // Вспомогательные методы
    void UpdateHistogramResults();
    
    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
    EVT_BUTTON(ID_HistogramGenerate, MainWindow::OnHistogramGenerate)
    EVT_BUTTON(ID_HistogramClear, MainWindow::OnHistogramClear)
    EVT_BUTTON(ID_AddIncident, MainWindow::OnAddIncident)
    EVT_BUTTON(ID_AddTask, MainWindow::OnAddTask)
    EVT_BUTTON(ID_PrioritizeTasks, MainWindow::OnPrioritizeTasks)
    EVT_BUTTON(ID_ClearTasks, MainWindow::OnClearTasks)
    EVT_BUTTON(ID_FindSubstrings, MainWindow::OnFindSubstrings)
    EVT_BUTTON(ID_ClearSubstrings, MainWindow::OnClearSubstrings)
    EVT_BUTTON(ID_AnalyzeFile, MainWindow::OnAnalyzeFile)
    EVT_BUTTON(ID_ClearFile, MainWindow::OnClearFile)
    EVT_MENU(wxID_EXIT, MainWindow::OnExit)
    EVT_MENU(wxID_ABOUT, MainWindow::OnAbout)
wxEND_EVENT_TABLE()

// Конструктор главного окна
MainWindow::MainWindow(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(1000, 750)) {
    
    // Устанавливаем шрифт с поддержкой Unicode для всего окна
    SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Arial")));
    
    // Создаем меню
    wxMenuBar* menuBar = new wxMenuBar;
    
    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(wxID_EXIT, wxT("Выход\tAlt-X"), wxT("Выход из приложения"));
    menuBar->Append(fileMenu, wxT("Файл"));
    
    wxMenu* helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT, wxT("О программе\tF1"), wxT("Информация о программе"));
    menuBar->Append(helpMenu, wxT("Справка"));
    
    SetMenuBar(menuBar);
    
    // Создаем статус бар 
    CreateStatusBar(2);
    SetStatusText(wxT("Лабораторная работа №2 - Организация и поиск данных"), 0);
    SetStatusText(wxT("Готово"), 1);
    
    // Создаем основную панель
    wxPanel* mainPanel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // Создаем notebook для переключения между задачами
    notebook = new wxNotebook(mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP);
    
    // Добавляем вкладки
    notebook->AddPage(CreateHistogramPanel(notebook), wxT("Гистограмма"), true);
    notebook->AddPage(CreateTaskPrioritizerPanel(notebook), wxT("Приоритезация задач"), false);
    notebook->AddPage(CreateSubstringsPanel(notebook), wxT("Поиск подстрок"), false);
    
    mainSizer->Add(notebook, 1, wxEXPAND | wxALL, 5);
    mainPanel->SetSizer(mainSizer);
    
    // Центрируем окно
    Centre();
}

//  ПАНЕЛЬ ГИСТОГРАММЫ 
wxPanel* MainWindow::CreateHistogramPanel(wxWindow* parent) {
    wxPanel* panel = new wxPanel(parent);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // Заголовок
    wxStaticText* title = new wxStaticText(panel, wxID_ANY, 
        wxT("ЗАДАЧА И-1: Построение гистограммы распределения данных"),
        wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
    title->SetFont(wxFont(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    mainSizer->Add(title, 0, wxEXPAND | wxALL, 10);
    
    wxStaticLine* line1 = new wxStaticLine(panel, wxID_ANY, wxDefaultPosition, wxSize(-1, 2));
    mainSizer->Add(line1, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
    
    // Панель ввода данных
    wxStaticBox* inputBox = new wxStaticBox(panel, wxID_ANY, wxT("Входные данные"));
    wxStaticBoxSizer* inputSizer = new wxStaticBoxSizer(inputBox, wxVERTICAL);
    
    wxBoxSizer* row1 = new wxBoxSizer(wxHORIZONTAL);
    row1->Add(new wxStaticText(panel, wxID_ANY, wxT("Данные (через запятую):")), 
              0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    histInputData = new wxTextCtrl(panel, wxID_ANY, wxT("1.2, 3.4, 5.6, 2.1, 7.8, 4.5, 3.2, 6.7, 8.9, 5.4"),
                                   wxDefaultPosition, wxSize(400, -1));
    row1->Add(histInputData, 1, wxEXPAND);
    inputSizer->Add(row1, 0, wxEXPAND | wxALL, 5);
    
    wxBoxSizer* row2 = new wxBoxSizer(wxHORIZONTAL);
    row2->Add(new wxStaticText(panel, wxID_ANY, wxT("Количество диапазонов:")), 
              0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    histNumRanges = new wxTextCtrl(panel, wxID_ANY, wxT("5"), wxDefaultPosition, wxSize(80, -1));
    row2->Add(histNumRanges, 0);
    inputSizer->Add(row2, 0, wxEXPAND | wxALL, 5);
    
    mainSizer->Add(inputSizer, 0, wxEXPAND | wxALL, 10);
    
    // Панель управления 
    wxBoxSizer* controlSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* btnGenerate = new wxButton(panel, ID_HistogramGenerate, wxT("Построить гистограмму"));
    wxButton* btnClear = new wxButton(panel, ID_HistogramClear, wxT("Очистить"));
    
    controlSizer->Add(btnGenerate, 0, wxRIGHT, 10);
    controlSizer->Add(btnClear, 0);
    
    mainSizer->Add(controlSizer, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
    
    // Результаты
    wxStaticBox* resultBox = new wxStaticBox(panel, wxID_ANY, wxT("Результаты"));
    wxStaticBoxSizer* resultSizer = new wxStaticBoxSizer(resultBox, wxVERTICAL);
    
    histResults = new wxListCtrl(panel, wxID_ANY, wxDefaultPosition, wxSize(-1, 200),
                                 wxLC_REPORT | wxLC_SINGLE_SEL | wxBORDER_SUNKEN);
    histResults->AppendColumn(wxT("Диапазон"), wxLIST_FORMAT_LEFT, 200);
    histResults->AppendColumn(wxT("Количество"), wxLIST_FORMAT_CENTER, 100);
    histResults->AppendColumn(wxT("Процент"), wxLIST_FORMAT_CENTER, 100);
    
    resultSizer->Add(histResults, 1, wxEXPAND | wxALL, 5);
    
    histStats = new wxStaticText(panel, wxID_ANY, wxT("Статистика: ожидание данных..."));
    histStats->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    resultSizer->Add(histStats, 0, wxEXPAND | wxALL, 5);
    
    mainSizer->Add(resultSizer, 1, wxEXPAND | wxALL, 10);
    
    panel->SetSizer(mainSizer);
    return panel;
}

//  ПАНЕЛЬ ПРИОРИТЕЗАЦИИ ЗАДАЧ 
wxPanel* MainWindow::CreateTaskPrioritizerPanel(wxWindow* parent) {
    wxPanel* panel = new wxPanel(parent);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // Заголовок
    wxStaticText* title = new wxStaticText(panel, wxID_ANY, 
        wxT("ЗАДАЧА И-3: Приоритезация задач на основе инцидентов"),
        wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
    title->SetFont(wxFont(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    mainSizer->Add(title, 0, wxEXPAND | wxALL, 10);
    
    wxStaticLine* line1 = new wxStaticLine(panel, wxID_ANY);
    mainSizer->Add(line1, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
    
    // Основной контейнер с тремя колонками
    wxBoxSizer* columnsSizer = new wxBoxSizer(wxHORIZONTAL);
    
    // Колонка 1: Инциденты
    wxStaticBox* incidentBox = new wxStaticBox(panel, wxID_ANY, wxT("Инциденты"));
    wxStaticBoxSizer* incidentSizer = new wxStaticBoxSizer(incidentBox, wxVERTICAL);
    
    incidentList = new wxListCtrl(panel, wxID_ANY, wxDefaultPosition, wxSize(300, 200),
                                  wxLC_REPORT | wxLC_SINGLE_SEL);
    incidentList->AppendColumn(wxT("Ключевые слова"), wxLIST_FORMAT_LEFT, 200);
    incidentList->AppendColumn(wxT("Приоритет"), wxLIST_FORMAT_CENTER, 80);
    incidentSizer->Add(incidentList, 1, wxEXPAND | wxALL, 5);
    
    wxButton* btnAddIncident = new wxButton(panel, ID_AddIncident, wxT("Добавить инцидент"));
    incidentSizer->Add(btnAddIncident, 0, wxALIGN_CENTER | wxALL, 5);
    
    columnsSizer->Add(incidentSizer, 1, wxEXPAND | wxALL, 5);
    
    // Колонка 2: Задачи
    wxStaticBox* taskBox = new wxStaticBox(panel, wxID_ANY, wxT("Задачи"));
    wxStaticBoxSizer* taskSizer = new wxStaticBoxSizer(taskBox, wxVERTICAL);
    
    taskList = new wxListCtrl(panel, wxID_ANY, wxDefaultPosition, wxSize(300, 200),
                              wxLC_REPORT | wxLC_SINGLE_SEL);
    taskList->AppendColumn(wxT("Название задачи"), wxLIST_FORMAT_LEFT, 150);
    taskList->AppendColumn(wxT("Ключевые слова"), wxLIST_FORMAT_LEFT, 130);
    taskSizer->Add(taskList, 1, wxEXPAND | wxALL, 5);
    
    wxButton* btnAddTask = new wxButton(panel, ID_AddTask, wxT("Добавить задачу"));
    taskSizer->Add(btnAddTask, 0, wxALIGN_CENTER | wxALL, 5);
    
    columnsSizer->Add(taskSizer, 1, wxEXPAND | wxALL, 5);
    
    mainSizer->Add(columnsSizer, 1, wxEXPAND | wxALL, 10);
    
    // Панель управления
    wxBoxSizer* controlSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* btnPrioritize = new wxButton(panel, ID_PrioritizeTasks, wxT("Выполнить приоритезацию"));
    wxButton* btnClear = new wxButton(panel, ID_ClearTasks, wxT("Очистить все"));
    
    controlSizer->Add(btnPrioritize, 0, wxRIGHT, 10);
    controlSizer->Add(btnClear, 0);
    
    mainSizer->Add(controlSizer, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
    
    // Результаты
    wxStaticBox* resultBox = new wxStaticBox(panel, wxID_ANY, wxT("Результаты приоритезации"));
    wxStaticBoxSizer* resultSizer = new wxStaticBoxSizer(resultBox, wxVERTICAL);
    
    prioritizedList = new wxListCtrl(panel, wxID_ANY, wxDefaultPosition, wxSize(-1, 150),
                                     wxLC_REPORT | wxLC_SINGLE_SEL);
    prioritizedList->AppendColumn(wxT("Место"), wxLIST_FORMAT_CENTER, 50);
    prioritizedList->AppendColumn(wxT("Задача"), wxLIST_FORMAT_LEFT, 200);
    prioritizedList->AppendColumn(wxT("Приоритет"), wxLIST_FORMAT_CENTER, 100);
    prioritizedList->AppendColumn(wxT("Ключевые слова"), wxLIST_FORMAT_LEFT, 250);
    
    resultSizer->Add(prioritizedList, 1, wxEXPAND | wxALL, 5);
    
    mainSizer->Add(resultSizer, 1, wxEXPAND | wxALL, 10);
    
    panel->SetSizer(mainSizer);
    return panel;
}

//  ПАНЕЛЬ ПОИСКА ПОДСТРОК 
wxPanel* MainWindow::CreateSubstringsPanel(wxWindow* parent) {
    wxPanel* panel = new wxPanel(parent);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // Заголовок
    wxStaticText* title = new wxStaticText(panel, wxID_ANY, 
        wxT("ЗАДАЧА И-9: Поиск наиболее частых подпоследовательностей"),
        wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
    title->SetFont(wxFont(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    mainSizer->Add(title, 0, wxEXPAND | wxALL, 10);
    
    wxStaticLine* line1 = new wxStaticLine(panel, wxID_ANY);
    mainSizer->Add(line1, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
    
    // Создаем notebook для двух вкладок
    substringsNotebook = new wxNotebook(panel, wxID_ANY);
    
    // Добавляем вкладки
    substringsNotebook->AddPage(CreateStringAnalysisPanel(substringsNotebook), wxT("Анализ строки"));
    substringsNotebook->AddPage(CreateFileAnalysisPanel(substringsNotebook), wxT("Анализ файла"));
    
    mainSizer->Add(substringsNotebook, 1, wxEXPAND | wxALL, 5);
    
    panel->SetSizer(mainSizer);
    return panel;
}

//  ВКЛАДКА "АНАЛИЗ СТРОКИ" 
wxPanel* MainWindow::CreateStringAnalysisPanel(wxWindow* parent) {
    wxPanel* panel = new wxPanel(parent);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // Панель ввода
    wxStaticBox* inputBox = new wxStaticBox(panel, wxID_ANY, wxT("Входные данные"));
    wxStaticBoxSizer* inputSizer = new wxStaticBoxSizer(inputBox, wxVERTICAL);
    
    wxBoxSizer* row1 = new wxBoxSizer(wxHORIZONTAL);
    row1->Add(new wxStaticText(panel, wxID_ANY, wxT("Исходная строка:")), 
              0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    substringInput = new wxTextCtrl(panel, wxID_ANY, wxT(""), 
                                     wxDefaultPosition, wxSize(400, -1));
    row1->Add(substringInput, 1, wxEXPAND);
    inputSizer->Add(row1, 0, wxEXPAND | wxALL, 5);
    
    wxBoxSizer* row2 = new wxBoxSizer(wxHORIZONTAL);
    row2->Add(new wxStaticText(panel, wxID_ANY, wxT("Мин. длина:")), 
              0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    minLengthCtrl = new wxTextCtrl(panel, wxID_ANY, wxT("2"), wxDefaultPosition, wxSize(60, -1));
    row2->Add(minLengthCtrl, 0, wxRIGHT, 10);
    
    row2->Add(new wxStaticText(panel, wxID_ANY, wxT("Макс. длина:")), 
              0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    maxLengthCtrl = new wxTextCtrl(panel, wxID_ANY, wxT("4"), wxDefaultPosition, wxSize(60, -1));
    row2->Add(maxLengthCtrl, 0, wxRIGHT, 10);
    
    row2->Add(new wxStaticText(panel, wxID_ANY, wxT("Топ N результатов:")), 
              0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    topNControl = new wxTextCtrl(panel, wxID_ANY, wxT("10"), wxDefaultPosition, wxSize(60, -1));
    row2->Add(topNControl, 0);
    inputSizer->Add(row2, 0, wxEXPAND | wxALL, 5);
    
    mainSizer->Add(inputSizer, 0, wxEXPAND | wxALL, 10);
    
    // Панель управления
    wxBoxSizer* controlSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* btnFind = new wxButton(panel, ID_FindSubstrings, wxT("Найти подстроки"));
    wxButton* btnClear = new wxButton(panel, ID_ClearSubstrings, wxT("Очистить"));
    
    controlSizer->Add(btnFind, 0, wxRIGHT, 10);
    controlSizer->Add(btnClear, 0);
    
    mainSizer->Add(controlSizer, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
    
    // Результаты
    wxStaticBox* resultBox = new wxStaticBox(panel, wxID_ANY, wxT("Результаты поиска"));
    wxStaticBoxSizer* resultSizer = new wxStaticBoxSizer(resultBox, wxVERTICAL);
    
    substringsResults = new wxListCtrl(panel, wxID_ANY, wxDefaultPosition, wxSize(-1, 250),
                                       wxLC_REPORT | wxLC_SINGLE_SEL);
    substringsResults->AppendColumn(wxT("№"), wxLIST_FORMAT_CENTER, 40);
    substringsResults->AppendColumn(wxT("Подстрока"), wxLIST_FORMAT_LEFT, 150);
    substringsResults->AppendColumn(wxT("Длина"), wxLIST_FORMAT_CENTER, 60);
    substringsResults->AppendColumn(wxT("Частота"), wxLIST_FORMAT_CENTER, 80);
    substringsResults->AppendColumn(wxT("Процент"), wxLIST_FORMAT_CENTER, 80);
    
    resultSizer->Add(substringsResults, 1, wxEXPAND | wxALL, 5);
    
    mainSizer->Add(resultSizer, 1, wxEXPAND | wxALL, 10);
    
    panel->SetSizer(mainSizer);
    return panel;
}

//  ВКЛАДКА "АНАЛИЗ ФАЙЛА" 
wxPanel* MainWindow::CreateFileAnalysisPanel(wxWindow* parent) {
    wxPanel* panel = new wxPanel(parent);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // Панель выбора файла
    wxStaticBox* fileBox = new wxStaticBox(panel, wxID_ANY, wxT("Выбор файла"));
    wxStaticBoxSizer* fileSizer = new wxStaticBoxSizer(fileBox, wxVERTICAL);
    
    wxBoxSizer* fileButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* btnSelectFile = new wxButton(panel, ID_AnalyzeFile, wxT("Выбрать файл"));
    fileNameLabel = new wxStaticText(panel, wxID_ANY, wxT("Файл не выбран"));
    fileNameLabel->SetForegroundColour(wxColour(100, 100, 100));
    fileButtonSizer->Add(btnSelectFile, 0, wxRIGHT, 10);
    fileButtonSizer->Add(fileNameLabel, 1, wxALIGN_CENTER_VERTICAL);
    
    fileSizer->Add(fileButtonSizer, 0, wxEXPAND | wxALL, 5);
    
    // Панель параметров
    wxBoxSizer* paramsSizer = new wxBoxSizer(wxHORIZONTAL);
    
    paramsSizer->Add(new wxStaticText(panel, wxID_ANY, wxT("Мин. длина:")), 
                     0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    fileMinLength = new wxSpinCtrl(panel, wxID_ANY, wxT("2"), wxDefaultPosition, wxSize(60, -1),
                                   wxSP_ARROW_KEYS, 1, 100, 2);
    paramsSizer->Add(fileMinLength, 0, wxRIGHT, 15);
    
    paramsSizer->Add(new wxStaticText(panel, wxID_ANY, wxT("Макс. длина:")), 
                     0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    fileMaxLength = new wxSpinCtrl(panel, wxID_ANY, wxT("4"), wxDefaultPosition, wxSize(60, -1),
                                   wxSP_ARROW_KEYS, 1, 100, 4);
    paramsSizer->Add(fileMaxLength, 0, wxRIGHT, 15);
    
    paramsSizer->Add(new wxStaticText(panel, wxID_ANY, wxT("Топ N результатов:")), 
                     0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    fileTopN = new wxSpinCtrl(panel, wxID_ANY, wxT("10"), wxDefaultPosition, wxSize(60, -1),
                              wxSP_ARROW_KEYS, 1, 100, 10);
    paramsSizer->Add(fileTopN, 0);
    
    fileSizer->Add(paramsSizer, 0, wxEXPAND | wxALL, 5);
    
    mainSizer->Add(fileSizer, 0, wxEXPAND | wxALL, 10);
    
    // Содержимое файла
    wxStaticBox* contentBox = new wxStaticBox(panel, wxID_ANY, wxT("Содержимое файла"));
    wxStaticBoxSizer* contentSizer = new wxStaticBoxSizer(contentBox, wxVERTICAL);
    
    fileContentText = new wxTextCtrl(panel, wxID_ANY, wxT(""), 
                                     wxDefaultPosition, wxSize(-1, 120),
                                     wxTE_MULTILINE | wxTE_READONLY);
    contentSizer->Add(fileContentText, 1, wxEXPAND | wxALL, 5);
    
    mainSizer->Add(contentSizer, 0, wxEXPAND | wxALL, 10);
    
    // Кнопка анализа
    wxBoxSizer* analyzeSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* btnAnalyze = new wxButton(panel, wxID_ANY, wxT("Проанализировать файл"));
    wxButton* btnClearFile = new wxButton(panel, ID_ClearFile, wxT("Очистить"));
    
    btnAnalyze->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) {
        if (fileContent.empty()) {
            wxMessageBox(wxT("Сначала выберите файл для анализа!"), 
                        wxT("Внимание"), wxOK | wxICON_WARNING);
            return;
        }
        
        try {
            int minLength = fileMinLength->GetValue();
            int maxLength = fileMaxLength->GetValue();
            int topN = fileTopN->GetValue();
            
            if (minLength <= 0 || maxLength <= 0 || minLength > maxLength) {
                wxMessageBox(wxT("Некорректные значения длин подстрок!"), 
                            wxT("Ошибка"), wxOK | wxICON_ERROR);
                return;
            }
            
            MostFrequentSubstrings fileAnalyzer;
            
            // Создание MutableArraySequence<char> для файла
            auto sequence = new MutableArraySequence<char>(static_cast<int>(fileContent.length()));
            for (size_t i = 0; i < fileContent.length(); i++) {
                sequence->Set(static_cast<int>(i), fileContent[i]);
            }
            
            // Поиск подстрок
            fileAnalyzer.FindMostFrequent(sequence, minLength, maxLength);
            
            // Получение топ-N результатов
            auto topSubstrings = fileAnalyzer.GetTopFrequent(topN);
            
            // Отображение результатов в таблице
            fileResultsList->DeleteAllItems();
            
            for (int i = 0; i < topSubstrings->GetLength(); i++) {
                auto sc = topSubstrings->Get(i);
                
                // Преобразование DynamicArray<char> в строку
                std::string substrStr = DynamicArrayToString(sc.substring);
                
                double percent = (sc.count * 100.0) / fileAnalyzer.GetTotalCount();
                
                long index = fileResultsList->InsertItem(fileResultsList->GetItemCount(), 
                                                         wxString::Format(wxT("%d"), i + 1));
                fileResultsList->SetItem(index, 1, StringToWxString(substrStr));
                fileResultsList->SetItem(index, 2, wxString::Format(wxT("%d"), sc.substring.GetSize()));
                fileResultsList->SetItem(index, 3, wxString::Format(wxT("%d"), sc.count));
                fileResultsList->SetItem(index, 4, wxString::Format(wxT("%.2f%%"), percent));
            }
            
            delete topSubstrings;
            delete sequence;
            
            SetStatusText(wxString::Format(wxT("Файл проанализирован. Найдено %d уникальных подстрок"), 
                          fileAnalyzer.GetUniqueCount()), 1);
            
        } catch (const std::exception& e) {
            wxMessageBox(wxString(wxT("Ошибка анализа файла: ")) + e.what(), 
                        wxT("Ошибка"), wxOK | wxICON_ERROR);
        }
    });
    
    analyzeSizer->Add(btnAnalyze, 0, wxRIGHT, 10);
    analyzeSizer->Add(btnClearFile, 0);
    
    mainSizer->Add(analyzeSizer, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
    
    // Результаты анализа файла
    wxStaticBox* resultsBox = new wxStaticBox(panel, wxID_ANY, wxT("Результаты анализа"));
    wxStaticBoxSizer* resultsSizer = new wxStaticBoxSizer(resultsBox, wxVERTICAL);
    
    fileResultsList = new wxListCtrl(panel, wxID_ANY, wxDefaultPosition, wxSize(-1, 200),
                                     wxLC_REPORT | wxLC_SINGLE_SEL);
    fileResultsList->AppendColumn(wxT("№"), wxLIST_FORMAT_CENTER, 40);
    fileResultsList->AppendColumn(wxT("Подстрока"), wxLIST_FORMAT_LEFT, 150);
    fileResultsList->AppendColumn(wxT("Длина"), wxLIST_FORMAT_CENTER, 60);
    fileResultsList->AppendColumn(wxT("Частота"), wxLIST_FORMAT_CENTER, 80);
    fileResultsList->AppendColumn(wxT("Процент"), wxLIST_FORMAT_CENTER, 80);
    
    resultsSizer->Add(fileResultsList, 1, wxEXPAND | wxALL, 5);
    
    mainSizer->Add(resultsSizer, 1, wxEXPAND | wxALL, 10);
    
    panel->SetSizer(mainSizer);
    return panel;
}

//  ОБРАБОТЧИКИ СОБЫТИЙ 

void MainWindow::OnHistogramGenerate(wxCommandEvent& event) {
    wxString input = histInputData->GetValue();
    if (input.IsEmpty()) {
        wxMessageBox(wxT("Введите данные для построения гистограммы!"), wxT("Ошибка"), wxOK | wxICON_WARNING);
        return;
    }
    
    try {
        // Разбор входных данных
        std::vector<double> data;
        std::stringstream ss(WxStringToString(input));
        std::string token;
        
        while (std::getline(ss, token, ',')) {
            // Удаляем пробелы
            token.erase(0, token.find_first_not_of(" \t"));
            token.erase(token.find_last_not_of(" \t") + 1);
            
            if (!token.empty()) {
                data.push_back(std::stod(token));
            }
        }
        
        if (data.empty()) {
            wxMessageBox(wxT("Некорректные данные!"), wxT("Ошибка"), wxOK | wxICON_ERROR);
            return;
        }
        
        // Определение минимального и максимального значений автоматически
        double minVal = *std::min_element(data.begin(), data.end());
        double maxVal = *std::max_element(data.begin(), data.end());
        
        int numRanges = wxAtoi(histNumRanges->GetValue());
        
        if (minVal == maxVal) {
            // Все значения одинаковые, немного расширяем диапазон для корректного отображения
            maxVal = minVal + 1.0;
        }
        
        if (numRanges <= 0) {
            wxMessageBox(wxT("Количество диапазонов должно быть больше 0!"), wxT("Ошибка"), wxOK | wxICON_ERROR);
            return;
        }
        
        // Создание MutableArraySequence<double> для гистограммы
        auto sequence = new MutableArraySequence<double>(static_cast<int>(data.size()));
        for (size_t i = 0; i < data.size(); i++) {
            sequence->Set(static_cast<int>(i), data[i]);
        }
        
        // Построение гистограммы
        histogram.CreateUniformRanges(minVal, maxVal, numRanges);
        histogram.BuildHistogramMapReduce(sequence);
        
        delete sequence;
        
        // Обновление результатов
        UpdateHistogramResults();
        
        SetStatusText(wxString::Format(wxT("Гистограмма построена успешно (диапазон: %.2f - %.2f)"), minVal, maxVal), 1);
        
    } catch (const std::exception& e) {
        wxMessageBox(wxString(wxT("Ошибка обработки данных: введите числовые данные")) , 
                    wxT("Ошибка"), wxOK | wxICON_ERROR);
    }
}

void MainWindow::UpdateHistogramResults() {
    histResults->DeleteAllItems();
    
    const auto& counts = histogram.GetCounts();
    int total = histogram.GetTotalCount();
    
    if (total == 0) {
        histStats->SetLabel(wxT("Статистика: нет данных"));
        return;
    }
    
    // Получаем ключи и значения из Dictionary
    auto keys = counts.GetKeys();
    
    for (int i = 0; i < keys->GetLength(); i++) {
        // Преобразуем DynamicArray<char> в std::string
        DynamicArray<char> rangeKey = keys->Get(i);
        std::string rangeStr = DynamicArrayToString(rangeKey);
        
        int count = counts.Get(rangeKey);
        double percent = (count * 100.0) / total;
        
        long index = histResults->InsertItem(histResults->GetItemCount(), StringToWxString(rangeStr));
        histResults->SetItem(index, 1, wxString::Format(wxT("%d"), count));
        histResults->SetItem(index, 2, wxString::Format(wxT("%.2f%%"), percent));
    }
    
    delete keys;
    
    // Обновление статистики
    DynamicArray<char> mostFrequent = histogram.GetMostFrequentRange();
    DynamicArray<char> leastFrequent = histogram.GetLeastFrequentRange();
    
    // Преобразуем в строки для вывода
    std::string mostFreqStr = DynamicArrayToString(mostFrequent);
    std::string leastFreqStr = DynamicArrayToString(leastFrequent);
    
    wxString stats = wxString::Format(
        wxT("Статистика: всего элементов: %d, уникальных диапазонов: %d, ")
        wxT("самый частый диапазон: %s, самый редкий: %s"),
        total, 
        counts.GetCount(),
        mostFreqStr.c_str(),
        leastFreqStr.c_str()
    );
    
    histStats->SetLabel(stats);
}

void MainWindow::OnHistogramClear(wxCommandEvent& event) {
    histInputData->Clear();
    histResults->DeleteAllItems();
    histStats->SetLabel(wxT("Статистика: данные очищены"));
    histogram.Clear();
    SetStatusText(wxT("Данные гистограммы очищены"), 1);
}

void MainWindow::OnAddIncident(wxCommandEvent& event) {
    wxTextEntryDialog dialog(this, 
        wxT("Введите ключевые слова инцидента (через запятую):"), 
        wxT("Добавление инцидента"));
    
    if (dialog.ShowModal() != wxID_OK) return;
    
    wxString keywordsStr = dialog.GetValue();
    if (keywordsStr.IsEmpty()) return;
    
    wxNumberEntryDialog prioDialog(this, 
        wxT("Введите приоритет инцидента (1-10):"), 
        wxT("Приоритет инцидента"), 
        wxT("Приоритет"), 1, 1, 10);
    
    if (prioDialog.ShowModal() != wxID_OK) return;
    int priority = prioDialog.GetValue();
    
    // Разбираем ключевые слова
    std::vector<std::string> keywords;
    std::stringstream ss(WxStringToString(keywordsStr));
    std::string token;
    
    while (std::getline(ss, token, ',')) {
        token.erase(0, token.find_first_not_of(" \t"));
        token.erase(token.find_last_not_of(" \t") + 1);
        if (!token.empty()) {
            keywords.push_back(token);
        }
    }
    
    // Сохраняем инцидент
    incidentData.push_back(std::make_pair(keywords, priority));
    
    // Добавляем в список
    long index = incidentList->InsertItem(incidentList->GetItemCount(), keywordsStr);
    incidentList->SetItem(index, 1, wxString::Format(wxT("%d"), priority));
    
    SetStatusText(wxT("Инцидент добавлен"), 1);
}

void MainWindow::OnAddTask(wxCommandEvent& event) {
    wxTextEntryDialog nameDialog(this, 
        wxT("Введите название задачи:"), 
        wxT("Добавление задачи"));
    
    if (nameDialog.ShowModal() != wxID_OK) return;
    wxString taskName = nameDialog.GetValue();
    
    wxTextEntryDialog keywordsDialog(this, 
        wxT("Введите ключевые слова задачи (через запятую):"), 
        wxT("Ключевые слова задачи"));
    
    if (keywordsDialog.ShowModal() != wxID_OK) return;
    wxString keywordsStr = keywordsDialog.GetValue();
    
    // Разбираем ключевые слова
    std::vector<std::string> keywords;
    std::stringstream ss(WxStringToString(keywordsStr));
    std::string token;
    
    while (std::getline(ss, token, ',')) {
        token.erase(0, token.find_first_not_of(" \t"));
        token.erase(token.find_last_not_of(" \t") + 1);
        if (!token.empty()) {
            keywords.push_back(token);
        }
    }
    
    // Сохраняем задачу
    taskData.push_back(std::make_pair(WxStringToString(taskName), keywords));
    
    // Добавляем в список
    long index = taskList->InsertItem(taskList->GetItemCount(), taskName);
    taskList->SetItem(index, 1, keywordsStr);
    
    SetStatusText(wxT("Задача добавлена"), 1);
}

void MainWindow::OnPrioritizeTasks(wxCommandEvent& event) {
    if (incidentData.empty() || taskData.empty()) {
        wxMessageBox(wxT("Добавьте хотя бы один инцидент и одну задачу!"), 
                    wxT("Ошибка"), wxOK | wxICON_WARNING);
        return;
    }
    
    try {
        // Создание MutableArraySequence<Incident>
        auto incidentsSeq = new MutableArraySequence<Incident>(static_cast<int>(incidentData.size()));
        for (size_t i = 0; i < incidentData.size(); i++) {
            DynamicArray<DynamicArray<char>> keywords = StringVectorToDynamicArray(incidentData[i].first);
            Incident incident(keywords, incidentData[i].second);
            incidentsSeq->Set(static_cast<int>(i), incident);
        }
        
        // Создание MutableArraySequence<Task>
        auto tasksSeq = new MutableArraySequence<Task>(static_cast<int>(taskData.size()));
        for (size_t i = 0; i < taskData.size(); i++) {
            DynamicArray<char> name = StringToDynamicArray(taskData[i].first);
            DynamicArray<DynamicArray<char>> keywords = StringVectorToDynamicArray(taskData[i].second);
            Task task(name, keywords);
            tasksSeq->Set(static_cast<int>(i), task);
        }
        
        // Вычисление приоритетов
        taskPrioritizer.CalculateKeywordPriorities(incidentsSeq);
        taskPrioritizer.PrioritizeTasks(tasksSeq);
        
        // Обновление результатов
        prioritizedList->DeleteAllItems();
        
        for (int i = 0; i < tasksSeq->GetLength(); i++) {
            const Task& task = tasksSeq->Get(i);
            
            // Получаем имя задачи из DynamicArray<char>
            std::string taskNameStr = DynamicArrayToString(task.name);
            
            long index = prioritizedList->InsertItem(prioritizedList->GetItemCount(), 
                                                    wxString::Format(wxT("%d"), i + 1));
            prioritizedList->SetItem(index, 1, StringToWxString(taskNameStr));
            prioritizedList->SetItem(index, 2, wxString::Format(wxT("%d"), task.calculatedPriority));
            
            // Формирование строки ключевых слов
            wxString keywords;
            for (int j = 0; j < task.keywords.GetSize(); j++) {
                DynamicArray<char> keyword = task.keywords.Get(j);
                keywords += StringToWxString(DynamicArrayToString(keyword));
                if (j < task.keywords.GetSize() - 1) keywords += wxT(", ");
            }
            prioritizedList->SetItem(index, 3, keywords);
        }
        
        delete incidentsSeq;
        delete tasksSeq;
        
        SetStatusText(wxT("Приоритезация выполнена успешно"), 1);
        
    } catch (const std::exception& e) {
        wxMessageBox(wxString(wxT("Ошибка приоритезации: ")) + e.what(), 
                    wxT("Ошибка"), wxOK | wxICON_ERROR);
    }
}

void MainWindow::OnClearTasks(wxCommandEvent& event) {
    incidentList->DeleteAllItems();
    taskList->DeleteAllItems();
    prioritizedList->DeleteAllItems();
    incidentData.clear();
    taskData.clear();
    taskPrioritizer.Clear();
    SetStatusText(wxT("Все данные очищены"), 1);
}

void MainWindow::OnFindSubstrings(wxCommandEvent& event) {
    wxString input = substringInput->GetValue();
    if (input.IsEmpty()) {
        wxMessageBox(wxT("Введите исходную строку!"), wxT("Ошибка"), wxOK | wxICON_WARNING);
        return;
    }
    
    try {
        int minLength = wxAtoi(minLengthCtrl->GetValue());
        int maxLength = wxAtoi(maxLengthCtrl->GetValue());
        int topN = wxAtoi(topNControl->GetValue());
        
        if (minLength <= 0 || maxLength <= 0 || minLength > maxLength) {
            wxMessageBox(wxT("Некорректные значения длин подстрок!"), wxT("Ошибка"), wxOK | wxICON_ERROR);
            return;
        }
        
        if (topN <= 0) topN = 10;
        
        // Создание MutableArraySequence<char>
        std::string inputStr = WxStringToString(input);
        auto sequence = new MutableArraySequence<char>(static_cast<int>(inputStr.length()));
        for (size_t i = 0; i < inputStr.length(); i++) {
            sequence->Set(static_cast<int>(i), inputStr[i]);
        }
        
        // Поиск подстрок
        substringFinder.FindMostFrequent(sequence, minLength, maxLength);
        
        // Получение топ-N результатов
        auto topSubstrings = substringFinder.GetTopFrequent(topN);
        
        // Отображение результатов
        substringsResults->DeleteAllItems();
        
        for (int i = 0; i < topSubstrings->GetLength(); i++) {
            auto sc = topSubstrings->Get(i);
            
            // Преобразование DynamicArray<char> в строку
            std::string substrStr = DynamicArrayToString(sc.substring);
            
            double percent = (sc.count * 100.0) / substringFinder.GetTotalCount();
            
            long index = substringsResults->InsertItem(substringsResults->GetItemCount(), 
                                                      wxString::Format(wxT("%d"), i + 1));
            substringsResults->SetItem(index, 1, StringToWxString(substrStr));
            substringsResults->SetItem(index, 2, wxString::Format(wxT("%d"), sc.substring.GetSize()));
            substringsResults->SetItem(index, 3, wxString::Format(wxT("%d"), sc.count));
            substringsResults->SetItem(index, 4, wxString::Format(wxT("%.2f%%"), percent));
        }
        
        delete topSubstrings;
        delete sequence;
        
        // Обновление статистики в статус баре
        SetStatusText(wxString::Format(wxT("Найдено %d уникальных подстрок"), 
                      substringFinder.GetUniqueCount()), 1);
        
    } catch (const std::exception& e) {
        wxMessageBox(wxString(wxT("Ошибка поиска подстрок: ")) + e.what(), 
                    wxT("Ошибка"), wxOK | wxICON_ERROR);
    }
}

void MainWindow::OnClearSubstrings(wxCommandEvent& event) {
    substringInput->Clear();
    substringsResults->DeleteAllItems();
    substringFinder.Clear();
    SetStatusText(wxT("Данные поиска подстрок очищены"), 1);
}

void MainWindow::OnAnalyzeFile(wxCommandEvent& event) {
    wxFileDialog openFileDialog(this, wxT("Выберите файл для анализа"), wxT(""), wxT(""), 
                               wxT("Текстовые файлы (*.txt)|*.txt|Все файлы (*.*)|*.*"), 
                               wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    
    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;
    
    wxString filename = openFileDialog.GetPath();
    currentFileName = openFileDialog.GetFilename();
    fileNameLabel->SetLabel(currentFileName);
    fileNameLabel->SetForegroundColour(wxColour(0, 0, 0));
    
    try {
        // Читаем файл
        std::ifstream file(std::string(filename.utf8_str()), std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Не удалось открыть файл");
        }
        
        fileContent = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        
        // Отображаем содержимое файла
        wxString fileContentWx = wxString::FromUTF8(fileContent.c_str());
        if (fileContentWx.IsEmpty() && !fileContent.empty()) {
            fileContentWx = wxString(fileContent.c_str(), wxConvLocal);
        }
        
        // Ограничиваем отображение (первые 5000 символов для производительности)
        if (fileContentWx.Length() > 5000) {
            fileContentWx = fileContentWx.Left(5000) + wxT("\n\n... (файл обрезан, всего символов: ") + 
                          wxString::Format(wxT("%d"), (int)fileContent.length()) + wxT(")");
        }
        
        fileContentText->SetValue(fileContentWx);
        fileResultsList->DeleteAllItems();
        
        SetStatusText(wxString::Format(wxT("Файл '%s' загружен (%d символов)"), 
                      currentFileName.c_str(), (int)fileContent.length()), 1);
        
    } catch (const std::exception& e) {
        wxMessageBox(wxString(wxT("Ошибка при загрузке файла: ")) + e.what(), 
                    wxT("Ошибка"), wxOK | wxICON_ERROR);
        fileContentText->Clear();
        fileResultsList->DeleteAllItems();
        fileContent.clear();
    }
}

void MainWindow::OnClearFile(wxCommandEvent& event) {
    fileContentText->Clear();
    fileResultsList->DeleteAllItems();
    fileNameLabel->SetLabel(wxT("Файл не выбран"));
    fileNameLabel->SetForegroundColour(wxColour(100, 100, 100));
    currentFileName.Clear();
    fileContent.clear();
    SetStatusText(wxT("Данные файла очищены"), 1);
}

void MainWindow::OnExit(wxCommandEvent& event) {
    Close(true);
}

void MainWindow::OnAbout(wxCommandEvent& event) {
    wxString aboutText = 
        wxT("Лабораторная работа №2\n")
        wxT("«Информатика (организация и поиск данных)»\n\n")
        wxT("Реализованные задачи:\n")
        wxT("• И-1: Построение гистограммы\n")
        wxT("• И-3: Приоритезация задач\n")
        wxT("• И-9: Поиск наиболее частых подпоследовательностей\n\n");
    
    wxMessageBox(aboutText, wxT("О программе"), wxOK | wxICON_INFORMATION);
}

//  ТОЧКА ВХОДА 
class Lab2App : public wxApp {
public:
    virtual bool OnInit() {
        // Инициализация локали для поддержки русских символов
        wxLocale::AddCatalogLookupPathPrefix(wxT("."));
        wxLocale* locale = new wxLocale(wxLANGUAGE_RUSSIAN);
        locale->AddCatalog(wxT("lab2_app"));
        wxInitAllImageHandlers();
        
        MainWindow* frame = new MainWindow(wxT("Лабораторная работа №2 - Организация и поиск данных"));
        frame->Show(true);
        
        return true;
    }
};

wxIMPLEMENT_APP(Lab2App);