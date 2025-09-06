#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

// Estructura para almacenar las estadísticas de la CPU
typedef struct {
    long long user;
    long long nice;
    long long system;
    long long idle;
    long long iowait;
    long long irq;
    long long softirq;
    long long steal;
    long long guest;
    long long guest_nice;
} cpu_stats_t;

// Estructura para almacenar las estadísticas del disco
typedef struct {
    unsigned long long reads_completed;
    unsigned long long sectors_read;
    unsigned long long writes_completed;
    unsigned long long sectors_written;
} disk_stats_t;

// Función para obtener las estadísticas de la CPU
int get_cpu_stats(cpu_stats_t *stats) {
    FILE *fp;
    char line[256];
    fp = fopen("/proc/stat", "r");
    if (fp == NULL) {
        perror("Error al abrir /proc/stat");
        return -1;
    }
    if (fgets(line, sizeof(line), fp) != NULL) {
        sscanf(line, "cpu %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld",
               &stats->user, &stats->nice, &stats->system, &stats->idle, &stats->iowait,
               &stats->irq, &stats->softirq, &stats->steal, &stats->guest, &stats->guest_nice);
    }
    fclose(fp);
    return 0;
}

// Función para obtener las estadísticas del disco
int get_disk_stats(disk_stats_t *stats) {
    FILE *fp;
    char line[256];
    char dev_name[32];
    unsigned int major, minor;
    int found = 0;

    fp = fopen("/proc/diskstats", "r");
    if (fp == NULL) {
        perror("Error al abrir /proc/diskstats");
        return -1;
    }

    // Buscar el disco principal (ej. sda)
    while (fgets(line, sizeof(line), fp) != NULL) {
        sscanf(line, "%u %u %s %llu %*u %llu %*u %llu %*u %llu",
               &major, &minor, dev_name, &stats->reads_completed, &stats->sectors_read,
               &stats->writes_completed, &stats->sectors_written);
        // Puedes cambiar "sda" por el nombre de tu disco principal si es diferente (ej. nvme0n1)
        if (strstr(dev_name, "sda") != NULL) {
            found = 1;
            break;
        }
    }
    fclose(fp);
    return found ? 0 : -1;
}

// Función para calcular el uso de CPU
double calculate_cpu_usage(cpu_stats_t prev, cpu_stats_t curr) {
    long long prev_idle = prev.idle + prev.iowait;
    long long curr_idle = curr.idle + curr.iowait;
    long long prev_non_idle = prev.user + prev.nice + prev.system + prev.irq + prev.softirq + prev.steal;
    long long curr_non_idle = curr.user + curr.nice + curr.system + curr.irq + curr.softirq + curr.steal;

    long long prev_total = prev_idle + prev_non_idle;
    long long curr_total = curr_idle + curr_non_idle;

    double total_diff = (double)(curr_total - prev_total);
    double idle_diff = (double)(curr_idle - prev_idle);

    if (total_diff == 0) {
        return 0.0;
    }

    return (total_diff - idle_diff) / total_diff * 100.0;
}

// Función para obtener las estadísticas de la memoria
int get_mem_stats(long *total, long *free) {
    FILE *fp;
    char line[256];
    *total = 0;
    *free = 0;

    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL) {
        perror("Error al abrir /proc/meminfo");
        return -1;
    }
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strstr(line, "MemTotal:") != NULL) {
            sscanf(line, "MemTotal: %ld kB", total);
        }
        if (strstr(line, "MemFree:") != NULL) {
            sscanf(line, "MemFree: %ld kB", free);
        }
    }
    fclose(fp);
    return 0;
}

int main() {
    FILE *csv_file;
    cpu_stats_t prev_cpu, curr_cpu;
    disk_stats_t prev_disk, curr_disk;
    long total_mem, free_mem, used_mem;
    double cpu_usage, read_speed_kb, write_speed_kb;
    const double SECTOR_SIZE_KB = 0.5; // 512 bytes = 0.5 KB
    const int INTERVAL_MS = 25;

    // Abrir el archivo CSV para escritura
    csv_file = fopen("monitor_data.csv", "w");
    if (csv_file == NULL) {
        perror("Error al crear el archivo CSV");
        return 1;
    }

    // Escribir el encabezado del CSV
    fprintf(csv_file, "Timestamp,CPU_Usage_Percent,Used_Memory_KB,Read_Speed_KB/s,Write_Speed_KB/s\n");

    // Primeras lecturas para inicializar
    get_cpu_stats(&prev_cpu);
    get_disk_stats(&prev_disk);
    usleep(INTERVAL_MS * 1000); 

    while (1) {
        // Obtener la marca de tiempo
        time_t now = time(NULL);
        char timestamp[20];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

        // Obtener estadísticas actuales
        get_cpu_stats(&curr_cpu);
        get_disk_stats(&curr_disk);
        get_mem_stats(&total_mem, &free_mem);

        // Calcular el uso de la CPU
        cpu_usage = calculate_cpu_usage(prev_cpu, curr_cpu);

        // Calcular la velocidad de disco
        read_speed_kb = ((double)(curr_disk.sectors_read - prev_disk.sectors_read) * SECTOR_SIZE_KB) / (INTERVAL_MS / 1000.0);
        write_speed_kb = ((double)(curr_disk.sectors_written - prev_disk.sectors_written) * SECTOR_SIZE_KB) / (INTERVAL_MS / 1000.0);
        used_mem = total_mem - free_mem;

        // Escribir los datos en el archivo CSV
        fprintf(csv_file, "%s,%.2f,%.2f,%f,%f\n", timestamp, cpu_usage, (double)used_mem, read_speed_kb, write_speed_kb);
        fflush(csv_file);

        // Actualizar las estadísticas para la siguiente iteración
        prev_cpu = curr_cpu;
        prev_disk = curr_disk;

        // Esperar el intervalo
        usleep(INTERVAL_MS * 1000);
    }

    fclose(csv_file);
    return 0;
}
