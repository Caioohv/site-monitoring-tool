#pragma once
using namespace std;

#include <string>
#include <fstream>
#include <mutex>
#include <unordered_map>
#include <chrono>
#include <vector>

/**
 * @brief Estrutura para armazenar estatísticas diárias de um site
 */
struct DailyStats {
    string date;
    string site_name;
    int error_count;
    int total_requests;
    
    DailyStats(const string& date, const string& site_name)
        : date(date), site_name(site_name), error_count(0), total_requests(0) {}
};

/**
 * @brief Sistema de log para o monitor
 * 
 * Responsabilidades:
 * - Registrar erros por site e data
 * - Manter apenas logs dos últimos 7 dias
 * - Thread-safe para operações concorrentes
 * - Formatar logs de forma legível
 */
class Logger {
  private:
    string log_file_path_;
    mutex log_mutex_;

    unordered_map<string, DailyStats> daily_stats_;
    string current_date_;

    string getCurrentDate() const;
    string formatDate(const chrono::system_clock::time_point& time) const;
    void loadExistingLogs();
    void writeLogEntry(const string& site_name, bool is_error);
    void cleanupOldLogs();
    bool isDateWithinLastWeek(const string& date) const;

  public:
    explicit Logger(const string& log_file_path = "data/log.txt");
    ~Logger();

    /**
     * @brief Registra uma requisição (sucesso ou erro)
     * @param site_name Nome do site
     * @param status_code Status code HTTP retornado
     */
    void logRequest(const string& site_name, int status_code);

    /**
     * @brief Registra uma verificação SSL
     * @param site_name Nome do site
     * @param days_until_expiry Dias até expiração do certificado
     */
    void logSSLCheck(const string& site_name, int days_until_expiry);

    /**
     * @brief Registra um alerta enviado
     * @param site_name Nome do site
     * @param alert_type Tipo de alerta ("ERROR" ou "SSL")
     */
    void logAlert(const string& site_name, const string& alert_type);

    /**
     * @brief Força a escrita das estatísticas atuais no arquivo
     */
    void flush();

    /**
     * @brief Obtém estatísticas dos últimos 7 dias
     * @return Mapa com estatísticas por site e data
     */
    unordered_map<string, vector<DailyStats>> getWeeklyStats() const;

    /**
     * @brief Obtém estatísticas do dia atual
     * @return Mapa com estatísticas por site
     */
    unordered_map<string, DailyStats> getTodaysStats() const;
};

