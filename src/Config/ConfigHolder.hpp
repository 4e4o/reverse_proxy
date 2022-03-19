#ifndef CONFIG_HOLDER_HPP
#define CONFIG_HOLDER_HPP

class ConfigInstance;

class ConfigHolder {
public:
    ConfigHolder();
    virtual ~ConfigHolder();

    const ConfigInstance *config() const;
    void setConfig(const ConfigInstance *config);

private:
    const ConfigInstance *m_config;
};

#endif // CONFIG_HOLDER_HPP
