#include <QTest>

class MCForceRegistryTest : public QObject
{
    Q_OBJECT

public:
    MCForceRegistryTest();

private slots:
    void testAddUpdateRemove();
    void testUpdateWithEnable();
    void testClear();

private:

};
