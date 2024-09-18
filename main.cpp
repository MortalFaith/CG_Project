#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "stdafx.h"

// 线段绘制算法
enum Algorithm {
    DDA,
    Bresenham,
    Midpoint
};

class LineDrawer : public QWidget {
    // Q_OBJECT;
private:
    bool hasStartPoint = false; // 是否有起点
    bool drawing = false;       // 是否正在绘制
    QPoint startPoint;          // 线段的起点
    QPoint endPoint;            // 线段的终点
    QVector<QLine> lines;       // 存储已绘制的直线段
    Algorithm algorithm = DDA;  // 当前选择的算法

protected:
    // 重写绘制事件
    void paintEvent(QPaintEvent* event) override 
    {
        QPainter painter(this);
        // 启用抗锯齿
        // painter.setRenderHint(QPainter::Antialiasing);
        
        // 每次刷新的时候重新绘制已存在直线
        QPen pen;
        pen.setColor(Qt::black);  // 设置线条颜色
        pen.setWidth(5);          // 设置线条宽度为5像素
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

        // 如果有起点，绘制线段的预览
        if (hasStartPoint) 
        {
            painter.setPen(Qt::gray); // 设置预览线段的颜色
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

        // 如果有起点和终点，绘制实际线段
        if (drawing)
        {
            painter.setPen(Qt::black); // 设置实际线段的颜色
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

    // DDA 算法
    //void DDA(QPainter& painter, int x1, int y1, int x2, int y2) {
    //    int dx = x2 - x1;
    //    int dy = y2 - y1;
    //    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
    //    float xIncrement = dx / (float)steps;
    //    float yIncrement = dy / (float)steps;
    //    float x = x1;
    //    float y = y1;
    //    for (int i = 0; i <= steps; i++) {
    //        painter.drawPoint(round(x), round(y)); // 在计算出的点位置绘制
    //        x += xIncrement;
    //        y += yIncrement;
    //    }
    //}

    // DDA 算法实现
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

    // Bresenham 算法实现
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

    // 中点算法实现
    void drawMidpointLine(QPainter& painter, QPoint p1, QPoint p2) {
        // 提取信息
        int x1 = p1.x();
        int y1 = p1.y();
        int x2 = p2.x();
        int y2 = p2.y();

        int x = x1, y = y1;	//赋初始点
        int dy = y1 - y2, dx = x2 - x1;
        int delta_x = (dx >= 0 ? 1 : (dx = -dx, -1));	//若dx>0则步长为1，否则为-1，同时dx变正
        int delta_y = (dy <= 0 ? 1 : (dy = -dy, -1));	//注意这里dy<0,才是画布中y的增长方向

        painter.drawPoint(x, y);		//画起始点

        int d, incrE, incrNE;
        if (-dy <= dx)		// 斜率绝对值 <= 1
            //这里-dy即画布中的dy
        {
            d = 2 * dy + dx;	//初始化判断式d
            incrE = 2 * dy;		//取像素E时判别式增量
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
        else				// 斜率绝对值 > 1
                            // x和y情况互换
        {
            d = 2 * dx + dy;
            incrE = 2 * dx;
            incrNE = 2 * (dy + dx);
            while (y != y2)
            {
                if (d < 0)	//注意d变化情况
                    d += incrE;
                else
                    x += delta_x, d += incrNE;
                y += delta_y;
                painter.drawPoint(x, y);
            }
        }
    }

    // 处理鼠标按下事件
    void mousePressEvent(QMouseEvent* event) override {
        if (!hasStartPoint) {
            // 第一次点击：记录起点
            startPoint = event->pos();
            hasStartPoint = true;
            endPoint = startPoint; // 初始化终点为起点
            drawing = false; // 初始化为不绘制实际直线
        }
    }

    // 处理鼠标松开事件
    void mouseReleaseEvent(QMouseEvent* event) override
    {
        if (hasStartPoint)
        {   // 松开鼠标，记录终点，开始绘制
            endPoint = event->pos();
            drawing = true;
            lines.append(QLine(startPoint, endPoint));
            update();   // 触发重绘
            hasStartPoint = false;  // 重置，允许再次绘制新的线段
        }
    }

    // 处理鼠标移动事件
    void mouseMoveEvent(QMouseEvent* event) override {
        if (hasStartPoint) 
        {
            // 更新终点为当前鼠标位置
            endPoint = event->pos();
            update(); // 触发重绘
        }
    }

public:
    LineDrawer(QWidget* parent = nullptr) : QWidget(parent) 
    {
        // 设置背景颜色
        QPalette pal = this->palette();
        pal.setColor(QPalette::Window, Qt::white); // 这里设置背景颜色为黄色
        this->setAutoFillBackground(true);  // 启用自动填充背景
        this->setPalette(pal);

        // setWindowTitle("DDA Line Drawing with Preview");
        // // 绘图窗口大小↓
        setFixedSize(500, 300);
    }

    void setAlgorithm(Algorithm algo) 
    {
        algorithm = algo;
        update(); // 改变算法后重新绘制
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

        // 创建下拉框并添加选项
        algorithmComboBox = new QComboBox(this);
        algorithmComboBox->addItem("DDA", DDA);
        algorithmComboBox->addItem("Bresenham", Bresenham);
        algorithmComboBox->addItem("Midpoint", Midpoint);

        // 布局管理器
        QVBoxLayout* rightLayout = new QVBoxLayout();
        rightLayout->addWidget(algorithmComboBox);
        rightLayout->addStretch();

        QHBoxLayout* mainLayout = new QHBoxLayout(this);
        mainLayout->addWidget(lineDrawer);
        mainLayout->addLayout(rightLayout);

        // 连接下拉框的选择变化信号
        connect(algorithmComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
            lineDrawer->setAlgorithm(static_cast<Algorithm>(algorithmComboBox->currentData().toInt()));
            });

        setLayout(mainLayout);
        setWindowTitle("Line Drawing with Algorithms");
        // 主窗口大小↓
        setFixedSize(600, 400);
    }
};


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}

