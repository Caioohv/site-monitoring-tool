#pragma once
//isso garante q o arquivo só será carregado uma vez na compilação

#include <string>
#include <vector>
#include <memory> // ponteiros inteligentes
#include <thread>
#include <mutex> // sincronia entre threads
#include <condition_variable> // ...
#include <atomic> //vatiáveis seguras para concorrência
#include <chrono> // manipulação de tempo
#include <unordered_map> //dicionario c hash
#include <queue> 

// Forward declarations para evitar includes desnecessários
class EndpointLoader;
class NotificationSystem;
class Logger;
class SSLChecker;

struct MonitoredSite {
  std::string url;
  std::string name;
  std::queue<int> last_status_codes;
  std::chrono::system_clock::time_point last_alert_time;
  bool in_cooldown;
  int consecutive_errors;

  MonitoredSite(const std::string& url, const std::string& name)
    : url(url), name(name), in_cooldown(false), consecutive_errors(0) {}
};

/**
 * @brief classe orquestradora
 * 
 * Esta classe coordena todo o sistema de monitoramento:
 * - carrega endpoints do txt
 * - consulta os sites a cada 30 segundos
 * - detecta anomalias e emite alertas
 * - verifica certificados SSL
 * - mantém logs de erros
 */
class SiteMonitor {
  private:
    static const int MAX_STATUS_HISTORY = 20;
    static const int CONSECUTIVE_ERRORS_THRESHOLD = 10;
    static const std::chrono::minutes COOLDOWN_DURATION;
    static const std::chrono::seconds MONITOR_INTERVAL;
    static const int SSL_WARNING_DAYS = 7;

    std::unique_ptr<EndpointLoader> endpoint_loader_;
    std::unique_ptr<NotificationSystem> notification_system_;
    std::unique_ptr<Logger> logger_;
    std::unique_ptr<SSLChecker> ssl_checker_;

    std::unordered_map<std::string, std::unique_ptr<MonitoredSite>> monitored_sites_;

    std::atomic<bool> running_;
    std::thread monitor_thread_;
    std::mutex sites_mutex_;
    std::condition_variable cv_;

    void monitorLoop();
    void checkSite(MonitoredSite& site);
    void updateStatusHistory(MonitoredSite& site, int status_code);
    bool shouldAlert(const MonitoredSite& site);
    void sendAlert(const MonitoredSite& site);
    void checkSSLCertificates();
    void loadEndpoints();
    void cleanupOldLogs();

  public:
    /**
     * @brief Construtor do monitor
     * @param config_file Caminho para o arquivo de configuração (sites.txt)
     */
    explicit SiteMonitor(const std::string& config_file = "data/sites.txt");
    
    /**
     * @brief Destrutor, garante que threads sejam finalizadas corretamente
     */
    ~SiteMonitor();
    
    /**
     * @brief Inicia o monitoramento
     * @return true se iniciado com sucesso, false caso contrário
     */
    bool start();
    
    /**
     * @brief Para o monitoramento
     */
    void stop();
    
    /**
     * @brief Verifica se o monitor está executando
     * @return true se executando, false caso contrário
     */
    bool isRunning() const { return running_.load(); }
    
    /**
     * @brief Adiciona um site no monitor
     * @param url URL do site
     * @param name Nome amigável do site
     */
    void addSite(const std::string& url, const std::string& name);
    
    /**
     * @brief Remove um site do monitor
     * @param url URL do site a ser removido
     */
    void removeSite(const std::string& url);
    
    /**
     * @brief Obtém estatísticas atuais
     * @return Mapa com estatísticas por site
     */
    std::unordered_map<std::string, int> getStats() const;

};