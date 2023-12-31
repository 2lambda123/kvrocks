#pragma once
#include <sys/resource.h>

#include <string>
#include <map>
#include <vector>
#include <set>

#include <rocksdb/options.h>

#include "config_type.h"
#include "status.h"
#include "cron.h"

// forward declaration
class Server;
namespace Engine {
class Storage;
}

#define SUPERVISED_NONE 0
#define SUPERVISED_AUTODETECT 1
#define SUPERVISED_SYSTEMD 2
#define SUPERVISED_UPSTART 3

const size_t KiB = 1024L;
const size_t MiB = 1024L * KiB;
const size_t GiB = 1024L * MiB;

extern const char *kDefaultNamespace;

struct CompactionCheckerRange {
 public:
  int Start;
  int Stop;

  bool Enabled() {
    return Start != -1 || Stop != -1;
  }
};

struct Config{
 public:
  Config();
  ~Config();
  int port = 6666;
  int repl_port = 66667;
  int workers = 0;
  int repl_workers = 1;
  int timeout = 0;
  int loglevel = 0;
  int backlog = 511;
  int maxclients = 10000;
  int max_backup_to_keep = 1;
  int max_backup_keep_hours = 168;
  int slowlog_log_slower_than = 100000;
  int slowlog_max_len = 128;
  bool daemonize = false;
  int supervised_mode = SUPERVISED_NONE;
  bool slave_readonly = true;
  bool slave_serve_stale_data = true;
  int slave_priority = 100;
  int max_db_size = 0;
  int max_replication_mb = 0;
  int max_io_mb = 0;
  bool codis_enabled = false;
  bool auto_resize_block_and_sst = true;

  std::vector<std::string> binds;
  std::vector<std::string> repl_binds;
  std::string dir;
  std::string db_dir;
  std::string backup_dir;
  std::string log_dir;
  std::string pidfile;
  std::string db_name;
  std::string masterauth;
  std::string requirepass;
  std::string master_host;
  int master_port = 0;
  Cron compact_cron;
  Cron bgsave_cron;
  CompactionCheckerRange compaction_checker_range{-1, -1};
  std::map<std::string, std::string> tokens;

  // profiling
  int profiling_sample_ratio = 0;
  int profiling_sample_record_threshold_ms = 0;
  int profiling_sample_record_max_len = 128;
  std::set<std::string> profiling_sample_commands;
  bool profiling_sample_all_commands = false;

  struct {
    int block_size;
    bool cache_index_and_filter_blocks;
    int metadata_block_cache_size;
    int subkey_block_cache_size;
    int max_open_files;
    int write_buffer_size;
    int max_write_buffer_number;
    int max_background_compactions;
    int max_background_flushes;
    int max_sub_compactions;
    int stats_dump_period_sec;
    bool enable_pipelined_write;
    int64_t delayed_write_rate;
    int compaction_readahead_size;
    int target_file_size_base;
    int WAL_ttl_seconds;
    int WAL_size_limit_MB;
    int max_total_wal_size;
    int level0_slowdown_writes_trigger;
    int level0_stop_writes_trigger;
    int compression;
    bool disable_auto_compactions;
  } RocksDB;

 public:
  Status Rewrite();
  Status Load(std::string path);
  void Get(std::string key, std::vector<std::string> *values);
  Status Set(Server *svr, std::string key, const std::string &value);
  void SetMaster(const std::string &host, int port);
  void ClearMaster();
  Status GetNamespace(const std::string &ns, std::string *token);
  Status AddNamespace(const std::string &ns, const std::string &token);
  Status SetNamespace(const std::string &ns, const std::string &token);
  Status DelNamespace(const std::string &ns);

 private:
  std::string path_;
  std::string binds_;
  std::string repl_binds_;
  std::string slaveof_;
  std::string compact_cron_;
  std::string bgsave_cron_;
  std::string compaction_checker_range_;
  std::string profiling_sample_commands_;
  std::map<std::string, ConfigField*> fields_;

  void initFieldValidator();
  void initFieldCallback();
  Status parseConfigFromString(std::string input);
  Status finish();
  Status isNamespaceLegal(const std::string &ns);
};
