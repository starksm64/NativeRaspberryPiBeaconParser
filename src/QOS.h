#ifndef QOS_H
#define QOS_H

enum MqttQOS {
    // QOS = 0
            AT_MOST_ONCE,
    // OQS = 1
            AT_LEAST_ONCE,
    // QOS = 2
            EXACTLY_ONCE
};
#endif
