class Root {
public:
    auto executer;
    auto task_list;
    std::vector<auto> exclusive_task_group;

    // Constructor
    Root() { }

    // Methods
    void print() const {
        // Print class data here
    }
};

class Task_list {
public:
    std::vector<auto> resident_group;
    std::vector<auto> standby_group;

    // Constructor
    Task_list() { }

    // Methods
    void print() const {
        // Print class data here
    }
};

class Executer {
public:
    std::string package_name;
    auto task_setting;
    auto system_setting;

    // Constructor
    Executer() { }

    // Methods
    void print() const {
        // Print class data here
    }
};

class System_setting {
public:
    int priority;
    std::vector<auto> executer_cpu_affinity;
    std::vector<auto> idle_task_cpu_affinity;

    // Constructor
    System_setting() { }

    // Methods
    void print() const {
        // Print class data here
    }
};

class Task_setting {
public:
    std::string timer_type;
    int period;

    // Constructor
    Task_setting() { }

    // Methods
    void print() const {
        // Print class data here
    }
};

