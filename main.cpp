#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtGui/QPainter>
#include <QtGui/QImage>
#include <QtGui/QKeyEvent>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtCore/QTimer>
#include <QtCore/QElapsedTimer>
#include <QtCore/QFile>
#include <QtCore/QMap>
#include <QtCore/QStringList>
#include <QtCore/QDebug>

struct ObjInfo
{
    int x;
    int y;
    int w;
    int h;
};

enum TankDirection
{
    Up, Left, Down, Right
};

class Window : public QWidget
{
    Q_OBJECT
public:
    Window()
        : _horizontalInput(0)
        , _verticalInput(0)
        , _tankDirectionX(0)
        , _tankDirectionY(0)
    {
        setWindowTitle("Tanks");
        resize(256, 256);

        _tiles.load(":/assets/sprites/tiles.png");

        QDomDocument xmlDoc;
        QFile file(":/assets/sprites/tiles.xml");
        file.open(QIODevice::ReadOnly);
        xmlDoc.setContent(&file);
        file.close();

        QDomElement root = xmlDoc.documentElement();
        QDomElement sprite = root.firstChildElement();
        while (!sprite.isNull())
        {
            QString name = sprite.attribute("n");
            _objects[name] = ObjInfo();
            _objects[name].x = sprite.attribute("x").toInt();
            _objects[name].y = sprite.attribute("y").toInt();
            _objects[name].w = sprite.attribute("w").toInt();
            _objects[name].h = sprite.attribute("h").toInt();
            sprite = sprite.nextSiblingElement();
        }

        QFile levelFile(":/assets/sprites/level.csv");
        levelFile.open(QIODevice::ReadOnly);
        int rowCounter = 0, columnCounter = 0;
        while (!levelFile.atEnd())
        {
            QByteArray line = levelFile.readLine();
            QList<QByteArray> row = line.split(',');
            for (int i = 0; i < row.length(); i++)
            {
                int elem = row[i].toInt();
                _level[rowCounter][columnCounter++] = elem;
            }
            rowCounter++;
            columnCounter = 0;
        }

        connect(&_timer, &QTimer::timeout, this, &Window::gameLoop);
        _timer.start(1000.f/60.f);
        _elapsedTimer.start();
    }

private:
    QImage _tiles;
    float _tankX = 16;
    float _tankY = 16;
    float _speed = 50.f;
    QTimer _timer;
    QElapsedTimer _elapsedTimer;
    float _deltaTime;
    QMap<QString, ObjInfo> _objects;
    QString _tankUp01 = "tank_yellow_small_up_01.png";
    QString _tankUp02 = "tank_yellow_small_up_02.png";
    QString _tankLeft01 = "tank_yellow_small_left_01.png";
    QString _tankLeft02 = "tank_yellow_small_left_02.png";
    QString _tankDown01 = "tank_yellow_small_down_01.png";
    QString _tankDown02 = "tank_yellow_small_down_02.png";
    QString _tankRight01 = "tank_yellow_small_right_01.png";
    QString _tankRight02 = "tank_yellow_small_right_02.png";
    QString _currentTankAnim = _tankDown01;
    float _tankAnimation = 0.f;
    QString grayBlock = "gray_block.png";
    QString brick01 = "brick1.png";
    QString brick02 = "brick2.png";
    int _level[32][32];
    int _horizontalInput;
    int _verticalInput;
    int _tankDirectionX;
    int _tankDirectionY;

    void addHorizontalInput(int input)
    {
        _horizontalInput += input;
        setDx(qBound(-1, _horizontalInput, 1));
    }

    void addVerticalInput(int input)
    {
        _verticalInput += input;
        setDy(qBound(-1, _verticalInput, 1));
    }

    void setDx(int dx)
    {
        _tankDirectionX = dx;
    }

    void setDy(int dy)
    {
        _tankDirectionY = dy;
    }

    void keyPressEvent(QKeyEvent *event) override
    {
        if (event->isAutoRepeat())
        {
            return;
        }

        switch (event->key())
        {
            case Qt::Key_W:
            case Qt::Key_Up:
                addVerticalInput(1);
                if (_horizontalInput == 0 && _verticalInput != 0)
                {
                    _currentTankAnim = _tankUp01;
//                    _tankX = 16.f + int((_tankX - 16.f) / 16.f) * 16.0f;
//                    qDebug() << int((_tankX - 16.f) / 8.f);
                    int a = int((_tankX - 16.f) / 8.f);
                    if (a % 2 != 0)
                    {
                        a++;
                    }
                    _tankX = 16.f + a * 8.f;
                }
                break;
            case Qt::Key_S:
            case Qt::Key_Down:
                addVerticalInput(-1);
                if (_horizontalInput == 0 && _verticalInput != 0)
                {
                    _currentTankAnim = _tankDown01;
                    int a = int((_tankX - 16.f) / 8.f);
                    if (a % 2 != 0)
                    {
                        a++;
                    }
                    _tankX = 16.f + a * 8.f;
                }
                break;
            case Qt::Key_A:
            case Qt::Key_Left:
                addHorizontalInput(-1);
                if (_verticalInput == 0 && _horizontalInput != 0)
                {
                    _currentTankAnim = _tankLeft01;
                    int a = int((_tankY - 16.f) / 8.f);
                    if (a % 2 != 0)
                    {
                        a++;
                    }
                    _tankY = 16.f + a * 8.f;
                }
                break;
            case Qt::Key_D:
            case Qt::Key_Right:
                addHorizontalInput(1);
                if (_verticalInput == 0 && _horizontalInput != 0)
                {
                    _currentTankAnim = _tankRight01;
                    int a = int((_tankY - 16.f) / 8.f);
                    if (a % 2 != 0)
                    {
                        a++;
                    }
                    _tankY = 16.f + a * 8.f;
                }
                break;
        }
    }

    void keyReleaseEvent(QKeyEvent *event) override
    {
        if (event->isAutoRepeat())
        {
            return;
        }

        switch (event->key())
        {
            case Qt::Key_W:
            case Qt::Key_Up:
                addVerticalInput(-1);
                break;
            case Qt::Key_S:
            case Qt::Key_Down:
                addVerticalInput(1);
                break;
            case Qt::Key_A:
            case Qt::Key_Left:
                addHorizontalInput(1);
                break;
            case Qt::Key_D:
            case Qt::Key_Right:
                addHorizontalInput(-1);
                break;
        }

        if (_horizontalInput == 0 && _verticalInput == 1)
        {
            _currentTankAnim = _tankUp01;
        }
        else if (_horizontalInput == 0 && _verticalInput == -1)
        {
            _currentTankAnim = _tankDown01;
        }
        else if (_horizontalInput == -1 && _verticalInput == 0)
        {
            _currentTankAnim = _tankLeft01;
        }
        else if (_horizontalInput == 1 && _verticalInput == 0)
        {
            _currentTankAnim = _tankRight01;
        }
    }

    void paintEvent(QPaintEvent *e) override
    {
        Q_UNUSED(e);
        QPainter qp(this);
        qp.setBrush(QBrush(QColor(0, 0, 0)));
        qp.drawRect(0, 0, width(), height());
        for (int r = 0; r < 32; r++)
        {
            for (int c = 0; c < 32; c++)
            {
                if (_level[r][c] == 92)
                {
                    qp.drawImage(r * 8, c * 8, _tiles, _objects[grayBlock].x, _objects[grayBlock].y, _objects[grayBlock].w, _objects[grayBlock].h);
                }
                else if (_level[r][c] == 1665 || _level[r][c] == 1764)
                {
                    qp.drawImage(r * 8, c * 8, _tiles, _objects[brick01].x, _objects[brick01].y, _objects[brick01].w, _objects[brick01].h);
                }
                else if (_level[r][c] == 1664 || _level[r][c] == 1765)
                {
                    qp.drawImage(r * 8, c * 8, _tiles, _objects[brick02].x, _objects[brick02].y, _objects[brick02].w, _objects[brick02].h);
                }
            }
        }
        qp.drawImage(_tankX, _tankY, _tiles, _objects[_currentTankAnim].x, _objects[_currentTankAnim].y, _objects[_currentTankAnim].w, _objects[_currentTankAnim].h);
    }

private slots:

    void gameLoop()
    {
        _deltaTime = _elapsedTimer.elapsed() / 1000.f;
        _elapsedTimer.restart();

        _tankAnimation += _deltaTime;
        if (_tankAnimation >= 0.1f)
        {
            _tankAnimation = 0.f;

            if (_tankDirectionY == 1 && _tankDirectionX == 0)
            {
                if (_currentTankAnim == _tankUp01)
                {
                    _currentTankAnim = _tankUp02;
                }
                else
                {
                    _currentTankAnim = _tankUp01;
                }
            }
            if (_tankDirectionX == -1 && _tankDirectionY == 0)
            {
                if (_currentTankAnim == _tankLeft01)
                {
                    _currentTankAnim = _tankLeft02;
                }
                else
                {
                    _currentTankAnim = _tankLeft01;
                }
            }
            if (_tankDirectionY == -1 && _tankDirectionX == 0)
            {
                if (_currentTankAnim == _tankDown01)
                {
                    _currentTankAnim = _tankDown02;
                }
                else
                {
                    _currentTankAnim = _tankDown01;
                }
            }
            if (_tankDirectionX == 1 && _tankDirectionY == 0)
            {
                if (_currentTankAnim == _tankRight01)
                {
                    _currentTankAnim = _tankRight02;
                }
                else
                {
                    _currentTankAnim = _tankRight01;
                }
            }
        }

        if (_tankDirectionY == 1 && _tankDirectionX == 0) // W or Up
        {
            _tankY -= _speed * _deltaTime;
        }
        if (_tankDirectionX == -1 && _tankDirectionY == 0) // A or Left
        {
            _tankX -= _speed * _deltaTime;
        }
        if (_tankDirectionY == -1 && _tankDirectionX == 0) // S or Down
        {
            _tankY += _speed * _deltaTime;
        }
        if (_tankDirectionX == 1 && _tankDirectionY == 0) // D or Right
        {
            _tankX += _speed * _deltaTime;
        }

        update();
    }
};

#include "main.moc"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Window w;
    w.show();
    return a.exec();
}
