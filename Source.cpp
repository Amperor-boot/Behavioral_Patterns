#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <Windows.h>

// Completing the 1st task: Command Pattern
class LogCommand
{
public:
    virtual ~LogCommand() = default;
    virtual void print(const std::string& message) = 0;
};

class ConsoleLogCommand : public LogCommand {
public:
    void print(const std::string& message) override {
        std::cout << message << std::endl;
    }
};

class FileLogCommand : public LogCommand
{
    std::string filePath;
public:
    FileLogCommand(const std::string& path) : filePath(path) {}
    void print(const std::string& message) override {
        std::ofstream outFile(filePath, std::ios::app);
        if (outFile.is_open()) {
            outFile << message << std::endl;
            outFile.close();
        }
    }
};

void executeLog(LogCommand& command, const std::string& message)
{
    command.print(message);
}

// Completing the 2nd task: Observer Pattern
class Observer
{
public:
    virtual void onWarning(const std::string& message) {}
    virtual void onError(const std::string& message) {}
    virtual void onFatalError(const std::string& message) {}
};

class Subject
{
    std::vector<Observer*> observers;
public:
    void addObserver(Observer* observer) {
        observers.push_back(observer);
    }

    void notifyWarning(const std::string& message) {
        for (auto& obs : observers)
            obs->onWarning(message);
    }

    void notifyError(const std::string& message) {
        for (auto& obs : observers)
            obs->onError(message);
    }

    void notifyFatalError(const std::string& message) {
        for (auto& obs : observers)
            obs->onFatalError(message);
    }
};

class ConsoleWarningObserver : public Observer
{
public:
    void onWarning(const std::string& message) override {
        std::cout << "Warning: " << message << std::endl;
    }
};

class FileErrorObserver : public Observer
{
    std::string filePath;
public:
    FileErrorObserver(const std::string& path) : filePath(path) {}
    void onError(const std::string& message) override {
        std::ofstream outFile(filePath, std::ios::app);
        if (outFile.is_open()) {
            outFile << "Error: " << message << std::endl;
            outFile.close();
        }
    }
};

class FatalErrorObserver : public Observer
{
    std::string filePath;
public:
    FatalErrorObserver(const std::string& path) : filePath(path) {}
    void onFatalError(const std::string& message) override {
        std::cout << "Fatal Error: " << message << std::endl;
        std::ofstream outFile(filePath, std::ios::app);
        if (outFile.is_open()) {
            outFile << "Fatal Error: " << message << std::endl;
            outFile.close();
        }
    }
};

// Completing the 3rd task: Chain of Responsibility Pattern
class LogMessage
{
public:
    enum class Type { Warning, Error, FatalError, Unknown };

private:
    Type type_;
    std::string message_;
public:
    LogMessage(Type type, const std::string& message) : type_(type), message_(message) {}

    Type type() const { return type_; }
    const std::string& message() const { return message_; }
};

class Handler
{
protected:
    std::shared_ptr<Handler> nextHandler;
public:
    void setNextHandler(std::shared_ptr<Handler> handler) {
        nextHandler = handler;
    }

    virtual void handle(const LogMessage& message) {
        if (nextHandler) {
            nextHandler->handle(message);
        }
    }
};

class FatalErrorHandler : public Handler
{
public:
    void handle(const LogMessage& message) override {
        if (message.type() == LogMessage::Type::FatalError) {
            throw std::runtime_error(message.message());
        }
        else {
            Handler::handle(message);
        }
    }
};

class ErrorHandler : public Handler
{
public:
    void handle(const LogMessage& message) override {
        if (message.type() == LogMessage::Type::Error) {
            std::ofstream outFile("errors.log", std::ios::app);
            if (outFile.is_open()) {
                outFile << "Error: " << message.message() << std::endl;
                outFile.close();
            }
        }
        else {
            Handler::handle(message);
        }
    }
};

class WarningHandler : public Handler
{
public:
    void handle(const LogMessage& message) override {
        if (message.type() == LogMessage::Type::Warning) {
            std::cout << "Warning: " << message.message() << std::endl;
        }
        else {
            Handler::handle(message);
        }
    }
};

class UnknownHandler : public Handler
{
public:
    void handle(const LogMessage& message) override {
        if (message.type() == LogMessage::Type::Unknown) {
            throw std::runtime_error("Unknown message: " + message.message());
        }
        else {
            Handler::handle(message);
        }
    }
};

// Use case
int main(int argc, char** argv)
{
    setlocale(LC_CTYPE, "RU");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    // Задание 1  
    ConsoleLogCommand consoleLog;
    FileLogCommand fileLog("log.txt");
    executeLog(consoleLog, "Logging to console");
    executeLog(fileLog, "Logging to file");

    // Задание 2  
    Subject subject;
    ConsoleWarningObserver warningObserver;
    FileErrorObserver errorObserver("errors.log");
    FatalErrorObserver fatalObserver("fatal_errors.log");

    subject.addObserver(&warningObserver);
    subject.addObserver(&errorObserver);
    subject.addObserver(&fatalObserver);

    subject.notifyWarning("This is a warning");
    subject.notifyError("This is an error");
    subject.notifyFatalError("This is a fatal error");

    // Задание 3  
    auto fatalHandler = std::make_shared<FatalErrorHandler>();
    auto errorHandler = std::make_shared<ErrorHandler>();
    auto warningHandler = std::make_shared<WarningHandler>();
    auto unknownHandler = std::make_shared<UnknownHandler>();

    fatalHandler->setNextHandler(errorHandler);
    errorHandler->setNextHandler(warningHandler);
    warningHandler->setNextHandler(unknownHandler);

    try {
        LogMessage msg(LogMessage::Type::FatalError, "Fatal error occurred");
        fatalHandler->handle(msg);
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }

    return 0;
}
