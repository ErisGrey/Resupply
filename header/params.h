#pragma once
#include <string>

class Param {
protected:
    Param(const std::string instance_name = "", const int prob = 1) :
        instance_name_(instance_name),
        time_limit_(3600),
        prob_(prob),
        horizon_ratio_(1.0),
        gap_(0.15)
    {}

    static Param* singleton_;

    std::string instance_name_;
    int time_limit_;
    int prob_;
    double horizon_ratio_;
    double gap_;

public:
    Param(const Param& other) = delete;
    void operator=(const Param&) = delete;

    static Param* getInstance(const std::string& instance_name = "", const int prob = 1);

    int getTimeLimit() const {
        return time_limit_;
    }

    int getProbId() const {
        return prob_;
    }

    double getHorizonRatio() const {
        return horizon_ratio_;
    }

    double getGap() const {
        return gap_;
    }
    void setHorizonRatio(double b) {
        horizon_ratio_ = b;
    }
    void setTimelimit(int b) {
        time_limit_ = b;
    }
    void setGap(double b) {
        gap_ = b;
    }
    std::string getInstanceName() const {
        return instance_name_;
    }

    ~Param() {
        delete singleton_;
    }
};

Param* Param::singleton_ = nullptr;

Param* Param::getInstance(const std::string& instance_name, const int prob) {
    if (singleton_ == nullptr) {
        singleton_ = new Param(instance_name, prob);
    }
    return singleton_;
}