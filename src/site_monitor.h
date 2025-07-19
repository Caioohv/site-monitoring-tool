#pragma once
using namespace std;

#include <string>
#include <vector>
#include <memory> 
#include <thread>
#include <mutex> 
#include <condition_variable> 
#include <atomic> 
#include <chrono> 
#include <unordered_map>
#include <queue> 

// Forward declarations para evitar includes desnecessários
class EndpointLoader;
class NotificationSystem;
class Logger;
class SSLChecker;

struct MonitoredSite {
  string url;
  string name;
  queue<int> last_status_codes;
  chrono::system_clock::time_point last_alert_time;
  bool in_cooldown;
  int consecutive_errors;

  MonitoredSite(const string& url, const string& name)
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
    static const chrono::minutes COOLDOWN_DURATION;
    static const chrono::seconds MONITOR_INTERVAL;
    static const int SSL_WARNING_DAYS = 7;

    unique_ptr<EndpointLoader> endpoint_loader_;
    unique_ptr<NotificationSystem> notification_system_;
    unique_ptr<Logger> logger_;
    unique_ptr<SSLChecker> ssl_checker_;

    unordered_map<string, unique_ptr<MonitoredSite>> monitored_sites_;

    atomic<bool> running_;
    thread monitor_thread_;
    mutex sites_mutex_;
    condition_variable cv_;

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
    explicit SiteMonitor(const string& config_file = "data/sites.txt");
    
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
    void addSite(const string& url, const string& name);
    
    /**
     * @brief Remove um site do monitor
     * @param url URL do site a ser removido
     */
    void removeSite(const string& url);
    
    /**
     * @brief Obtém estatísticas atuais
     * @return Mapa com estatísticas por site
     */
    unordered_map<string, int> getStats() const;

};