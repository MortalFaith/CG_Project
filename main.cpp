#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "stdafx.h"

// �߶λ����㷨
enum Algorithm {
    DDA,
    Bresenham,
    Midpoint
};

class LineDrawer : public QWidget {
    // Q_OBJECT;
private:
    bool hasStartPoint = false; // �Ƿ������
    bool drawing = false;       // �Ƿ����ڻ���
    QPoint startPoint;          // �߶ε����
    QPoint endPoint;            // �߶ε��յ�
    QVector<QLine> lines;       // �洢�ѻ��Ƶ�ֱ�߶�
    Algorithm algorithm = DDA;  // ��ǰѡ����㷨

protected:
    // ��д�����¼�
    void paintEvent(QPaintEvent* event) override 
    {
        QPainter painter(this);
        // ���ÿ����
        // painter.setRenderHint(QPainter::Antialiasing);
        
        // ÿ��ˢ�µ�ʱ�����»����Ѵ���ֱ��
        QPen pen;
        pen.setColor(Qt::black);  // ����������ɫ
        pen.setWidth(5);          // �����������Ϊ5����
        painter.setPen(pen);

        for (const QLine& line : lines) {
            switch (algorithm) {
            case DDA:
                drawDDALine(painter, line.p1(), line.p2());
                break;
            case Bresenham:
                drawBresenhamLine(painter, line.p1(), line.p2());
                break;
            case Midpoint:
                drawMidpointLine(painter, line.p1(), line.p2());
                break;
            }
        }

        // �������㣬�����߶ε�Ԥ��
        if (hasStartPoint) 
        {
            painter.setPen(Qt::gray); // ����Ԥ���߶ε���ɫ
            switch (algorithm) {
            case DDA:
                drawDDALine(painter, startPoint, endPoint);
                break;
            case Bresenham:
                drawBresenhamLine(painter, startPoint, endPoint);
                break;
            case Midpoint:
                drawMidpointLine(painter, startPoint, endPoint);
                break;
            }
        }

        // ����������յ㣬����ʵ���߶�
        if (drawing)
        {
            painter.setPen(Qt::black); // ����ʵ���߶ε���ɫ
            switch (algorithm) {
            case DDA:
                drawDDALine(painter, startPoint, endPoint);
                break;
            case Bresenham:
                drawBresenhamLine(painter, startPoint, endPoint);
                break;
            case Midpoint:
                drawMidpointLine(painter, startPoint, endPoint);
                break;
            }
        }
    }

    // DDA �㷨
    //void DDA(QPainter& painter, int x1, int y1, int x2, int y2) {
    //    int dx = x2 - x1;
    //    int dy = y2 - y1;
    //    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
    //    float xIncrement = dx / (float)steps;
    //    float yIncrement = dy / (float)steps;
    //    float x = x1;
    //    float y = y1;
    //    for (int i = 0; i <= steps; i++) {
    //        painter.drawPoint(round(x), round(y)); // �ڼ�����ĵ�λ�û���
    //        x += xIncrement;
    //        y += yIncrement;
    //    }
    //}

    // DDA �㷨ʵ��
    void drawDDALine(QPainter& painter, QPoint p1, QPoint p2) {
        int dx = p2.x() - p1.x();
        int dy = p2.y() - p1.y();
        int steps = std::max(abs(dx), abs(dy));
        float xIncrement = dx / static_cast<float>(steps);
        float yIncrement = dy / static_cast<float>(steps);

        float x = p1.x();
        float y = p1.y();
        for (int i = 0; i <= steps; ++i) {
            painter.drawPoint(static_cast<int>(x), static_cast<int>(y));
            x += xIncrement;
            y += yIncrement;
        }
    }

    // Bresenham �㷨ʵ��
    void drawBresenhamLine(QPainter& painter, QPoint p1, QPoint p2) {
        int x1 = p1.x();
        int y1 = p1.y();
        int x2 = p2.x();
        int y2 = p2.y();
        int dx = abs(x2 - x1);
        int dy = abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int err = dx - dy;

        while (x1 != x2 || y1 != y2) {
            painter.drawPoint(x1, y1);
            int e2 = 2 * err;
            if (e2 > -dy) {
                err -= dy;
                x1 += sx;
            }
            if (e2 < dx) {
                err += dx;
                y1 += sy;
            }
        }
    }

    // �е��㷨ʵ��
    void drawMidpointLine(QPainter& painter, QPoint p1, QPoint p2) {
        // ��ȡ��Ϣ
        int x1 = p1.x();
        int y1 = p1.y();
        int x2 = p2.x();
        int y2 = p2.y();

        int x = x1, y = y1;	//����ʼ��
        int dy = y1 - y2, dx = x2 - x1;
        int delta_x = (dx >= 0 ? 1 : (dx = -dx, -1));	//��dx>0�򲽳�Ϊ1������Ϊ-1��ͬʱdx����
        int delta_y = (dy <= 0 ? 1 : (dy = -dy, -1));	//ע������dy<0,���ǻ�����y����������

        painter.drawPoint(x, y);		//����ʼ��

        int d, incrE, incrNE;
        if (-dy <= dx)		// б�ʾ���ֵ <= 1
            //����-dy�������е�dy
        {
            d = 2 * dy + dx;	//��ʼ���ж�ʽd
            incrE = 2 * dy;		//ȡ����Eʱ�б�ʽ����
            incrNE = 2 * (dy + dx);//NE
            while (x != x2)
            {
                if (d < 0)
                    y += delta_y, d += incrNE;
                else
                    d += incrE;
                x += delta_x;
                painter.drawPoint(x, y);
            }
        }
        else				// б�ʾ���ֵ > 1
                            // x��y�������
        {
            d = 2 * dx + dy;
            incrE = 2 * dx;
            incrNE = 2 * (dy + dx);
            while (y != y2)
            {
                if (d < 0)	//ע��d�仯���
                    d += incrE;
                else
                    x += delta_x, d += incrNE;
                y += delta_y;
                painter.drawPoint(x, y);
            }
        }
    }

    // ������갴���¼�
    void mousePressEvent(QMouseEvent* event) override {
        if (!hasStartPoint) {
            // ��һ�ε������¼���
            startPoint = event->pos();
            hasStartPoint = true;
            endPoint = startPoint; // ��ʼ���յ�Ϊ���
            drawing = false; // ��ʼ��Ϊ������ʵ��ֱ��
        }
    }

    // ��������ɿ��¼�
    void mouseReleaseEvent(QMouseEvent* event) override
    {
        if (hasStartPoint)
        {   // �ɿ���꣬��¼�յ㣬��ʼ����
            endPoint = event->pos();
            drawing = true;
            lines.append(QLine(startPoint, endPoint));
            update();   // �����ػ�
            hasStartPoint = false;  // ���ã������ٴλ����µ��߶�
        }
    }

    // ��������ƶ��¼�
    void mouseMoveEvent(QMouseEvent* event) override {
        if (hasStartPoint) 
        {
            // �����յ�Ϊ��ǰ���λ��
            endPoint = event->pos();
            update(); // �����ػ�
        }
    }

public:
    LineDrawer(QWidget* parent = nullptr) : QWidget(parent) 
    {
        // ���ñ�����ɫ
        QPalette pal = this->palette();
        pal.setColor(QPalette::Window, Qt::white); // �������ñ�����ɫΪ��ɫ
        this->setAutoFillBackground(true);  // �����Զ���䱳��
        this->setPalette(pal);

        // setWindowTitle("DDA Line Drawing with Preview");
        // // ��ͼ���ڴ�С��
        setFixedSize(500, 300);
    }

    void setAlgorithm(Algorithm algo) 
    {
        algorithm = algo;
        update(); // �ı��㷨�����»���
    }
};

class MainWindow : public QWidget {
    // Q_OBJECT

private:
    LineDrawer* lineDrawer;
    QComboBox* algorithmComboBox;

public:
    MainWindow(QWidget* parent = nullptr) : QWidget(parent) {
        lineDrawer = new LineDrawer(this);

        // �������������ѡ��
        algorithmComboBox = new QComboBox(this);
        algorithmComboBox->addItem("DDA", DDA);
        algorithmComboBox->addItem("Bresenham", Bresenham);
        algorithmComboBox->addItem("Midpoint", Midpoint);

        // ���ֹ�����
        QVBoxLayout* rightLayout = new QVBoxLayout();
        rightLayout->addWidget(algorithmComboBox);
        rightLayout->addStretch();

        QHBoxLayout* mainLayout = new QHBoxLayout(this);
        mainLayout->addWidget(lineDrawer);
        mainLayout->addLayout(rightLayout);

        // �����������ѡ��仯�ź�
        connect(algorithmComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
            lineDrawer->setAlgorithm(static_cast<Algorithm>(algorithmComboBox->currentData().toInt()));
            });

        setLayout(mainLayout);
        setWindowTitle("Line Drawing with Algorithms");
        // �����ڴ�С��
        setFixedSize(600, 400);
    }
};


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}

