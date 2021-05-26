#ifndef RESOURCES_H
#define RESOURCES_H

#include <QMap>
#include <QPixmap>

class Resources
{
protected:
    static Resources * singleton;
    Resources();
    QMap<QString, QPixmap> resources;
public:
    static Resources * getInstance() {
        if (singleton == nullptr) {
            singleton = new Resources();
        }
        return singleton;
    }
    QPixmap * getResource(QString resourceName) {
        return &resources[resourceName];
    }
};

#endif // RESOURCES_H
