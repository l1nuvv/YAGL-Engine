//
// Created by l1nuvv on 08.06.2025.
//

#include "Application.h"
#include "../platform/Window.h"
#include "../platform/Input.h"
#include "../render/Renderer.h"
#include "../utils/Logger.h"
#include <glm/glm.hpp>

Application *Application::s_instance = nullptr;

Application::Application(int width, int height, const std::string &title)
{
    // Проверяем что не создается второй экземлпяр
    if (s_instance) {
        LOG_ERROR("Application already exists!");
        return;
    }
    s_instance = this;

    // Проверка работы GLM
    LOG_DEBUG("GLM vector size for test: {}", sizeof(glm::vec3));

    // Создание основных компонентов движка в правильном порядке
    WindowProps windowProps(title, width, height, true);
    m_window   = std::make_unique<Window>(windowProps);
    m_renderer = std::make_unique<Renderer>();
}

Application::~Application()
{
    // Гарантируем корректное завершение работы движка
    ShutdownEngine();
    // Очищает singleton указатель
    s_instance = nullptr;
}

void Application::Run()
{
    InitializeEngine();

    // Проверка, что инициализация прошла успешно перед запуском основного цикла
    if (!m_running) {
        LOG_ERROR("Failed to initialize application!");
        return;
    }

    LOG_INFO("Running application...");

    // Основный цикл движка. Работает до сигнала завершения - ShutdownEngine()
    while (m_running && !m_window->ShouldClose()) {
        // Вычисляем время между кадрами для framerate независимых вычислений
        CalculateDeltaTime();

        // Обработка пользовательского ввода
        Input::ProcessInput();

        // Проверка запроса на закрытие от системы ввода
        if (Input::ShouldClose()) {
            m_running = false;
            break;
        }

        // Обновляем логику приложения с учетом времени кадра
        Update(m_deltaTime);

        // Очистка буфера кадра перед следующей отрисовкой
        m_renderer->Clear(glm::vec4(0.5f, 0.54f, 1.0f, 1.0f));

        // Выполнение отрисовки сцены
        Render();

        // Отображение отрисованного кадра пользователю
        m_window->SwapBuffers();
        //Обработка события окна
        m_window->Update();
    }

    LOG_INFO("Application exiting...");
    ShutdownEngine();
}

void Application::InitializeEngine()
{
    // Изначально приложение не запущено до успешной инициализации
    m_running = false;

    // Проверка, что окно успешно создано
    if (!m_window) {
        LOG_ERROR("Window not created!");
        return;
    }

    // Инициализация системы ввода привязкой к онку
    Input::Initialize(m_window->GetNativeWindow());

    // Установка callback для обработки изменения размера окна
    // Лямбда захватывает this для доступа к методам класса
    m_window->SetResizeCallback([this](int width, int height) {
        LOG_INFO("Window resized to {}x{}", width, height);
        // Уведомляем приложение об изменении размера окна
        OnWindowResize(width, height);
        // Обновляем viewport рендера, чтобы избежать искажений
        if (m_renderer) { m_renderer->SetViewport(width, height); }
    });

    // Инициализируем подсистему рендеринга изображения
    if (!m_renderer->Initialize()) {
        LOG_ERROR("Failed to initialize Renderer!");
        return;
    }

    // Вызываем пользовательскую инициализацию
    Initialize();

    // Ставим m_running = true, показывая что приложение готово к работе
    m_running = true;
    // Фиксируем время начала для расчета deltaTime
    m_lastFrameTime = static_cast<float>(glfwGetTime());
}

void Application::ShutdownEngine()
{
    // Вызываем пользовательскую очистку ресурсов
    Shutdown();

    // Освобождаем ресурсы в обратном порядку создания
    if (m_renderer) m_renderer.reset();

    if (m_window) m_window.reset();

    m_running = false;
}

void Application::CalculateDeltaTime()
{
    // Получаем текущее время и вычисляем разность с предыдущим кадром
    auto currentTime = static_cast<float>(glfwGetTime());
    m_deltaTime      = currentTime - m_lastFrameTime;
    m_lastFrameTime  = currentTime;
}
