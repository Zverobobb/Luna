#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>
#include <SFML/OpenGL.hpp>


using namespace sf;

// Высота окна, пиксель
const int height = 900;

// Ширина окна, пиксель
const int width = 1000;

// Число Пи
const double pi = 3.14159265;

// Число е
const double e = 2.718281828;

// Гравитационная постоянная, м3/(кг*с2)
const double G = 6.6743015 * pow(10, -11);

// Молярная масса воздуха, г/моль
const double Mv = 28.98;

// Универсальная газовая постоянная, Дж/(моль/К)
const double R = 8.314;

// Давление воздуха на уровне моря, Па
const double Pvm = 101325;

// Температура воздуха на данной высоте, К
const double Tvm[2][29] = {
    { 0, 500, 1000, 1500,  2000, 2500, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 11000, 12000, 14000, 16000, 18000, 20000, 24000, 28000, 32000, 36000, 40000, 50000, 60000, 80000, 100000 },
    { 288.2, 284.9, 281.7, 278.4, 275.2, 271.9, 268.7, 262.2, 255.7, 249.2, 242.7, 236.2, 229.7, 223.3, 216.8, 216.7, 216.7, 216.7, 216.7, 216.7, 220.6, 224.5, 228.5, 239.3, 250.4, 270.7, 247, 198.6, 196.6}
};
double TVm = Tvm[1][0];

// Плотность воздуха на уровне моря, кг/м3
const double  rvm = Pvm * Mv / R / TVm;

// Структура характеристик ступеней РН //
struct Stage {

    // Сухая масса ступени, кг
    double Ms;

    // Полная масса ступени, кг
    double M;

    // Масса топлива ступени, кг
    double Mt = Ms - M;

    // Тяга ступени на уровне моря, кН
    double Tm;

    // Тяга ступени в пустоте, кН
    double Tp;

    // Удельный импульс ступени на уровне моря, м/с
    double Im;

    // Удельный импульс ступени в пустоте, м/с
    double Ip;
};

// Структура планет и Солнца //
struct Planet {

    // Масса объекта, кг
    double M;

    // Перигелий орбиты, м
    double Rp;

    // Афелий орбиты, м
    double Ra;

    // Радиус объекта, м
    double R;

    // Проекция скорости на ось X, м/с
    double Vx;

    // Проекция скорости на ось Y, м/с
    double Vy;

    // Координата X объекта, м
    double x;

    // Координата Y объекта, м
    double y;

    // Проекции ускорения на оси X и Y, м/с
    double ax;
    double ay;

    // Ускорение свободного падения от Солнца, м/с2
    double gs;

    // Ускорение свободного падения от планеты (для спутников), м/с2
    double gp;

    // Ускорение свободного падения от Луны (для РН), м/с2
    double gl;

    // Угол относительно Солнца, рад
    double phis;

    // Угол отосительно планеты (для спутников), рад
    double phip;

    // Угол отосительно Луны (для РН), рад
    double phil;
};

double norm(double x, double y);
double dot(double x, double y, double Z[]);
double clamp(double value, double min, double max);

RenderWindow window(VideoMode(width, height), "Luna");

int main() {

    // Данные Солнца //

    Planet Sun;

    Sun.M = 1.9885 * pow(10, 30);
    Sun.R = 6.9551 * pow(10, 8);
    Sun.x = 0;
    Sun.y = 0;

    // Данные Земли //

    Planet Earth;

    Earth.M = 5.9726 * pow(10, 24);
    Earth.R = 6.371 * pow(10, 6);
    Earth.Rp = 1.4709829 * pow(10, 11);
    Earth.Ra = 1.52098232 * pow(10, 11);
    Earth.Vx = 2.9783 * pow(10, 4);
    Earth.Vy = 0;
    Earth.x = Sun.x;
    Earth.y = Sun.y + Earth.Rp;
    Earth.ax = 0;
    Earth.ay = 0;
    Earth.gs = 0;
    Earth.phis = 0;

    // Данные Луны //

    Planet Luna;

    Luna.M = 7.3477 * pow(10, 22);
    Luna.R = 1.7971 * pow(10, 6);
    Luna.Rp = 3.63104 * pow(10, 8);
    Luna.Ra = 4.05696 * pow(10, 8);
    Luna.Vx = Earth.Vx + 1.023 * pow(10, 3);
    Luna.Vy = Earth.Vy;
    Luna.x = Earth.x;
    Luna.y = Earth.y + Luna.Rp;
    Luna.ax = 0;
    Luna.ay = 0;
    Luna.gs = 0;
    Luna.gp = 0;
    Luna.phis = 0;
    Luna.phip = 0;

    // Данные Меркурия //

    Planet Mercury;

    Mercury.M = 3.33022 * pow(10, 23);
    Mercury.R = 2.4397 * pow(10, 6);
    Mercury.Rp = 4.6001009 * pow(10, 10);
    Mercury.Ra = 6.9817445 * pow(10, 10);
    Mercury.Vx = 4.736 * pow(10, 4);
    Mercury.Vy = 0;
    Mercury.x = Sun.x;
    Mercury.y = Sun.y + Mercury.Rp;
    Mercury.ax = 0;
    Mercury.ay = 0;
    Mercury.gs = 0;
    Mercury.phis = 0;

    // Данные Венеры //

    Planet Venus;

    Venus.M = 4.8675 * pow(10, 24);
    Venus.R = 6.0518 * pow(10, 6);
    Venus.Rp = 1.07476259 * pow(10, 11);
    Venus.Ra = 1.08942109 * pow(10, 11);
    Venus.Vx = 3.502 * pow(10, 4);
    Venus.Vy = 0;
    Venus.x = Sun.x;
    Venus.y = Sun.y + Venus.Rp;
    Venus.ax = 0;
    Venus.ay = 0;
    Venus.gs = 0;
    Venus.phis = 0;

    // Данные Марса //

    Planet Mars;

    Mars.M = 6.4171 * pow(10, 23);
    Mars.R = 3.3895 * pow(10, 6);
    Mars.Rp = 2.06655 * pow(10, 11);
    Mars.Ra = 2.49232 * pow(10, 11);
    Mars.Vx = 2.4077 * pow(10, 4);
    Mars.Vy = 0;
    Mars.x = Sun.x;
    Mars.y = Sun.y + Mars.Rp;
    Mars.ax = 0;
    Mars.ay = 0;
    Mars.gs = 0;
    Mars.phis = 0;

    // Данные Юпитера //

    Planet Jupiter;

    Jupiter.M = 1.8986 * pow(10, 27);
    Jupiter.R = 6.9911 * pow(10, 7);
    Jupiter.Rp = 7.405736 * pow(10, 11);
    Jupiter.Ra = 8.165208 * pow(10, 11);
    Jupiter.Vx = 1.307 * pow(10, 4);
    Jupiter.Vy = 0;
    Jupiter.x = Sun.x;
    Jupiter.y = Sun.y + Jupiter.Rp;
    Jupiter.ax = 0;
    Jupiter.ay = 0;
    Jupiter.gs = 0;
    Jupiter.phis = 0;

    // Данные Сатурна //

    Planet Saturn;

    Saturn.M = 5.6846 * pow(10, 26);
    Saturn.R = 5.8232 * pow(10, 7);
    Saturn.Rp = 1.353572956 * pow(10, 12);
    Saturn.Ra = 1.513325783 * pow(10, 12);
    Saturn.Vx = 9.69 * pow(10, 3);
    Saturn.Vy = 0;
    Saturn.x = Sun.x;
    Saturn.y = Sun.y + Saturn.Rp;
    Saturn.ax = 0;
    Saturn.ay = 0;
    Saturn.gs = 0;
    Saturn.phis = 0;

    // Данные Урана //

    Planet Uran;

    Uran.M = 8.6813 * pow(10, 25);
    Uran.R = 2.5362 * pow(10, 7);
    Uran.Rp = 2.748938461 * pow(10, 12);
    Uran.Ra = 3.004419704 * pow(10, 12);
    Uran.Vx = 6.81 * pow(10, 3);
    Uran.Vy = 0;
    Uran.x = Sun.x;
    Uran.y = Sun.y + Uran.Rp;
    Uran.ax = 0;
    Uran.ay = 0;
    Uran.gs = 0;
    Uran.phis = 0;

    // Данные Нептуна //

    Planet Neptune;

    Neptune.M = 1.0243 * pow(10, 26);
    Neptune.R = 2.4622 * pow(10, 7);
    Neptune.Rp = 4.452940833 * pow(10, 12);
    Neptune.Ra = 4.55394649 * pow(10, 12);
    Neptune.Vx = 5.4349 * pow(10, 3);
    Neptune.Vy = 0;
    Neptune.x = Sun.x;
    Neptune.y = Sun.y + Neptune.Rp;
    Neptune.ax = 0;
    Neptune.ay = 0;
    Neptune.gs = 0;
    Neptune.phis = 0;

    // Данные первой ступени //

    Stage Ein;

    Ein.Ms = 15136;
    Ein.M = 177562;
    Ein.Mt = Ein.M - Ein.Ms;
    Ein.Tm = 3354;
    Ein.Tp = 4085.2;
    Ein.Im = 2582.973;
    Ein.Ip = 3141.162;

    // Данные второй ступени //

    Stage Zwei;

    Zwei.Ms = 6545;
    Zwei.M = 99765;
    Zwei.Mt = Zwei.M - Zwei.Ms;
    Zwei.Tm = 792.5;
    Zwei.Tp = 990.2;
    Zwei.Im = 2528.037;
    Zwei.Ip = 3145.086;

    // Данные третьей ступени //

    Stage Drei;

    Drei.Ms = 2355;
    Drei.M = 27755;
    Drei.Mt = Drei.M - Drei.Ms;
    Drei.Tp = 294.3;
    Drei.Ip = 3521.79;

    // Данные разгонного блока //

    Stage Rb;

    Rb.Ms = 887;
    Rb.M = 6280;
    Rb.Mt = Rb.M - Rb.Ms;
    Rb.Tp = 19.9;
    Rb.Ip = 3268.692;

    // Данные аппарата //

    Stage A;

    A.Ms = 605;
    A.M = 1605;
    A.Mt = A.M - A.Ms;
    A.Tm = 4.7072;
    A.Tp = 0.5884;
    A.Ip = 3103.457;

    Planet Rocket;

    Rocket.M = Ein.M + Zwei.M + Drei.M + Rb.M + A.M;
    Rocket.x = Earth.x;
    Rocket.y = Earth.y + Earth.R;
    Rocket.Vx = Earth.Vx + 286.487;
    Rocket.Vy = Earth.Vy;
    Rocket.ax = 0;
    Rocket.ay = 0;
    Rocket.gs = 0;
    Rocket.gp = 0;
    Rocket.gl = 0;
    Rocket.phis = 0;
    Rocket.phip = 0;
    Rocket.phil = 0;


    // Тяга двигателей на уровне моря для рассчётов, кН
    double Tmm = Ein.Tm;

    // Тяга двигателей в пустоте для рассчётоы, кН
    double Tpp = Ein.Tp;

    // Удельный импульс двигателей на уровне моря для рассчётов, м/с
    double Imm = Ein.Im;

    // Удельный импульс двигателей в пустоте для рассчётов, м/с
    double Ipp = Ein.Ip;

    // Масса топлива ступени для рассчётов, кг
    double Mtt = Ein.Mt;

    // Полная тяга двигателей на высоте, кН
    double Ts = 0;

    // Полная удельная тяга на высоте, м/с
    double Is = 0;

    // Счётчик времени, с
    int T = 0;

    // Скорость течения времени, с/кадр
    int Tv = 0;

    // Коэффициент масштаба, 1/м
    double k = 5 / Jupiter.R;

    // Координаты курсора мыши
    double Mx, My, dMx = 0, dMy = 0;

    // Счётчик количества нажатий клавиши мыши
    int Tm = 0;

    // Дополнительный счётчик времени, с
    int Tvv = 0;

    // Координаты объекта, относительно которой происходит отрисовка, м
    double X = Sun.x, Y = Sun.y;
    
    // Переменная слежения за объектом
    int P = 0;

    // Давление воздуха на данной высоте, Па
    double Pv = 0;

    // Счётчик ступеней
    int s = 0;

    // Плотность воздуха на высоте, кг/м3
    double rv = 0;

    // Ускорение РН от двигателей, м/с2
    double ad = 0;

    // Отклонение вектора тяги двигателей относительно Земли, рад
    double alpha = 0;

    // Вектор направления исходной нормали
    double Z[] = { 0, 1 };

    // Счётчик времени полёта, с
    double t = 0;

    // Факт достигнутой высоты
    bool hg = 0;

    // Шаг времени, с
    double dt = 0.25;

    // Требуемые импульсы для гомановских орбит, м/с
    double Vg1 = 0, Vg2 = 0, Vg3 = 0;

    // Полученные импульсы для гомановских орбит, м/с
    double dV = 0;

    // Время начала импульсов, с
    double t1 = 3550, t2 = 340700, t3 = 346500, t4 = 349990;

    // Факт нужной скорости при спуске к Луне
    bool st = 0;

    while (window.isOpen()) {

        Event event;
        while (window.pollEvent(event)) {
            
            if (event.type == Event::Closed)
                window.close();

            // Изменение объекта, относительно которого происходит отрисовка
            if (Keyboard::isKeyPressed(Keyboard::Numpad0)) {
                P = 0;
                dMx = 0;
                dMy = 0;
            }
            else if (Keyboard::isKeyPressed(Keyboard::Numpad1)) {
                dMx = 0;
                dMy = 0;
                P = 1;
            }
            else if (Keyboard::isKeyPressed(Keyboard::Numpad2)) {
                P = 2;
                dMx = 0;
                dMy = 0;
            }
            else if (Keyboard::isKeyPressed(Keyboard::Numpad3)) {
                P = 3;
                dMx = 0;
                dMy = 0;
            }
            else if (Keyboard::isKeyPressed(Keyboard::Numpad4)) {
                P = 4;
                dMx = 0;
                dMy = 0;
            }
            else if (Keyboard::isKeyPressed(Keyboard::Numpad5)) {
                P = 5;
                dMx = 0;
                dMy = 0;
            }
            else if (Keyboard::isKeyPressed(Keyboard::Numpad6)) {
                P = 6;
                dMx = 0;
                dMy = 0;
            }
            else if (Keyboard::isKeyPressed(Keyboard::Numpad7)) {
                P = 7;
                dMx = 0;
                dMy = 0;
            }
            else if (Keyboard::isKeyPressed(Keyboard::Numpad8)) {
                P = 8;
                dMx = 0;
                dMy = 0;
            }
            else if (Keyboard::isKeyPressed(Keyboard::Numpad9)) {
                P = 9;
                dMx = 0;
                dMy = 0;
            }
            else if (Keyboard::isKeyPressed(Keyboard::Num0)) {
                P = 10;
                dMx = 0;
                dMy = 0;
            }

            // Изменение скорости течения времени
            if (Keyboard::isKeyPressed(Keyboard::Down) && Keyboard::isKeyPressed(Keyboard::LControl)) {
                    Tv -= 5;
                if (Tv < 0)
                    Tv = 0;
            }
            else if (Keyboard::isKeyPressed(Keyboard::Up) && Keyboard::isKeyPressed(Keyboard::LControl)) {
                if (Tv > 1000)
                    Tv += 50;
                else
                    Tv += 5;
            }
            else if (Keyboard::isKeyPressed(Keyboard::Down)) {
                if (Tv >= 1000)
                    Tv -= 500;
                else if (Tv >= 10000)
                    Tv -= 5000;
                else
                    Tv -= 50;
                    if (Tv < 0)
                        Tv = 0;
            }
            else if (Keyboard::isKeyPressed(Keyboard::Up)) {
                if (Tv >= 1000)
                    Tv += 500;
                else if (Tv >= 10000)
                    Tv += 5000;
                else
                    Tv += 50;
            }
            if (Keyboard::isKeyPressed(Keyboard::LAlt)) {
                if (Tv != 0) {
                    Tvv = Tv;
                    Tv = 0;
                }
                else if (Tvv != 0)
                    Tv = Tvv;
            }

            // Изменение масштаба
            if (event.type == Event::MouseWheelScrolled && Keyboard::isKeyPressed(Keyboard::LControl)) {
                if (event.mouseWheelScroll.delta < 0) {
                    k -= 0.1 / Earth.R;
                    if (k < 0)
                        k = 0;
                }
                else if (event.mouseWheelScroll.delta > 0)
                    k += 0.1 / Earth.R;
            }
            else if (event.type == Event::MouseWheelScrolled) {
                if (event.mouseWheelScroll.delta < 0) {
                    k -= 5 / Earth.R;
                    if (k < 0)
                        k = 0;
                }
                else if (event.mouseWheelScroll.delta > 0)
                    k += 5 / Earth.R;
            }

            // Перемещение по окну
            if (event.type == Event::MouseButtonPressed) {
                if (event.mouseButton.button == Mouse::Left) {
                    if (Tm % 2 == 0) {
                        Vector2i M = Mouse::getPosition(window);
                        Mx = M.x;
                        My = M.y;
                    }
                        Tm++;
                }
            }

            // Возврат камеры в изначальное положение
            if (Keyboard::isKeyPressed(Keyboard::R)) {
                dMx = 0;
                dMy = 0;
            }

            // При изменении размеров окна картинка не искажается
            if (event.type == Event::Resized) {
                FloatRect visibleArea(0.f, 0.f, event.size.width, event.size.height);
                window.setView(View(visibleArea));
            }
        }

        // Измненение объекта, относительно которого происходит отрисовка
        switch (P) {
        case 0:
            X = Sun.x;
            Y = Sun.y;
            break;
        case 1:
            X = Mercury.x;
            Y = Mercury.y;
            break;
        case 2:
            X = Venus.x;
            Y = Venus.y;
            break;
        case 3:
            X = Earth.x;
            Y = Earth.y;
            break;
        case 4:
            X = Mars.x;
            Y = Mars.y;
            break;
        case 5:
            X = Jupiter.x;
            Y = Jupiter.y;
            break;
        case 6:
            X = Saturn.x;
            Y = Saturn.y;
            break;
        case 7:
            X = Uran.x;
            Y = Uran.y;
            break;
        case 8:
            X = Neptune.x;
            Y = Neptune.y;
            break;
        case 9:
            X = Luna.x;
            Y = Luna.y;
            break;
        case 10:
            X = Rocket.x;
            Y = Rocket.y;
            break;
        }

        // Перемещение по окну
        if (Tm % 2 != 0) {
            Vector2i M = Mouse::getPosition(window);
            dMx += M.x - Mx;
            dMy += M.y - My;
            Mx = M.x;
            My = M.y;
        }

        // Чёрное пространство
        window.clear(Color::White);

        // Отриовка Солнца
        //CircleShape Sn(5.f);
        CircleShape Sn(Sun.R * k);
        Sn.setFillColor(Color::Yellow);
        Sn.setPosition(width / 2 - (Sun.R + X) * k + dMx, height / 2 - (Sun.R + Y) * k + dMy);
        //Sn.setPosition(width / 2 - 2.5f - X * k + dMx, height / 2 - 2.5f - Y * k + dMy);

        window.draw(Sn);

        // Отрисовка объектов //

        int T = 0;

        while (T < Tv) {

            t += dt;

            // Ускорения свободного падения //

            // РН
            Rocket.gs = G * Sun.M / pow(norm((Rocket.x - Sun.x), (Rocket.y - Sun.y)), 2);
            Rocket.gp = G * Earth.M / pow(norm((Rocket.x - Earth.x), (Rocket.y - Earth.y)), 2);
            Rocket.gl = G * Luna.M / pow(norm((Rocket.x - Luna.x), (Rocket.y - Luna.y)), 2);

            // Меркурий
            Mercury.gs = G * Sun.M / pow(norm((Mercury.x - Sun.x), (Mercury.y - Sun.y)), 2);

            // Венера
            Venus.gs = G * Sun.M / pow(norm((Venus.x - Sun.x), (Venus.y - Sun.y)), 2);

            // Земля
            Earth.gs = G * Sun.M / pow(norm((Earth.x - Sun.x), (Earth.y - Sun.y)), 2);

            // Луна
            Luna.gs = G * Sun.M / pow(norm((Luna.x - Sun.x), (Luna.y - Sun.y)), 2);
            Luna.gp = G * Earth.M / pow(norm((Luna.x - Earth.x), (Luna.y - Earth.y)), 2);

            // Марс
            Mars.gs = G * Sun.M / pow(norm((Mars.x - Sun.x), (Mars.y - Sun.y)), 2);

            // Юпитер
            Jupiter.gs = G * Sun.M / pow(norm((Jupiter.x - Sun.x), (Jupiter.y - Sun.y)), 2);

            // Сатурн
            Saturn.gs = G * Sun.M / pow(norm((Saturn.x - Sun.x), (Saturn.y - Sun.y)), 2);

            // Уран
            Uran.gs = G * Sun.M / pow(norm((Uran.x - Sun.x), (Uran.y - Sun.y)), 2);

            // Нептун
            Neptune.gs = G * Sun.M / pow(norm((Neptune.x - Sun.x), (Neptune.y - Sun.y)), 2);


            // Углы относительно Солнца //

            // РН
            if (Rocket.x >= Sun.x) {
                Rocket.phis = acos(dot((Rocket.x - Sun.x), (Rocket.y - Sun.y), Z) / norm((Rocket.x - Sun.x), (Rocket.y - Sun.y)) / norm(Z[0], Z[1]));
            }
            else {
                Rocket.phis = -acos(dot((Rocket.x - Sun.x), (Rocket.y - Sun.y), Z) / norm((Rocket.x - Sun.x), (Rocket.y - Sun.y)) / norm(Z[0], Z[1]));
            }

            // Меркурий
            if (Mercury.x >= Sun.x) {
                Mercury.phis = acos(dot((Mercury.x - Sun.x), (Mercury.y - Sun.y), Z) / norm((Mercury.x - Sun.x), (Mercury.y - Sun.y)) / norm(Z[0], Z[1]));
            }
            else {
                Mercury.phis = -acos(dot((Mercury.x - Sun.x), (Mercury.y - Sun.y), Z) / norm((Mercury.x - Sun.x), (Mercury.y - Sun.y)) / norm(Z[0], Z[1]));
            }

            // Венера
            if (Venus.x >= Sun.x) {
                Venus.phis = acos(dot((Venus.x - Sun.x), (Venus.y - Sun.y), Z) / norm((Venus.x - Sun.x), (Venus.y - Sun.y)) / norm(Z[0], Z[1]));
            }
            else {
                Venus.phis = -acos(dot((Venus.x - Sun.x), (Venus.y - Sun.y), Z) / norm((Venus.x - Sun.x), (Venus.y - Sun.y)) / norm(Z[0], Z[1]));
            }

            // Земля
            if (Earth.x >= Sun.x) {
                Earth.phis = acos(dot((Earth.x - Sun.x), (Earth.y - Sun.y), Z) / norm((Earth.x - Sun.x), (Earth.y - Sun.y)) / norm(Z[0], Z[1]));
            }
            else {
                Earth.phis = -acos(dot((Earth.x - Sun.x), (Earth.y - Sun.y), Z) / norm((Earth.x - Sun.x), (Earth.y - Sun.y)) / norm(Z[0], Z[1]));
            }

            // Луна
            if (Luna.x >= Sun.x) {
                Luna.phis = acos(dot((Luna.x - Sun.x), (Luna.y - Sun.y), Z) / norm((Luna.x - Sun.x), (Luna.y - Sun.y)) / norm(Z[0], Z[1]));
            }
            else {
                Luna.phis = -acos(dot((Luna.x - Sun.x), (Luna.y - Sun.y), Z) / norm((Luna.x - Sun.x), (Luna.y - Sun.y)) / norm(Z[0], Z[1]));
            }

            // Марс
            if (Mars.x >= Sun.x) {
                Mars.phis = acos(dot((Mars.x - Sun.x), (Mars.y - Sun.y), Z) / norm((Mars.x - Sun.x), (Mars.y - Sun.y)) / norm(Z[0], Z[1]));
            }
            else {
                Mars.phis = -acos(dot((Mars.x - Sun.x), (Mars.y - Sun.y), Z) / norm((Mars.x - Sun.x), (Mars.y - Sun.y)) / norm(Z[0], Z[1]));
            }

            // Юпитер
            if (Jupiter.x >= Sun.x) {
                Jupiter.phis = acos(dot((Jupiter.x - Sun.x), (Jupiter.y - Sun.y), Z) / norm((Jupiter.x - Sun.x), (Jupiter.y - Sun.y)) / norm(Z[0], Z[1]));
            }
            else {
                Jupiter.phis = -acos(dot((Jupiter.x - Sun.x), (Jupiter.y - Sun.y), Z) / norm((Jupiter.x - Sun.x), (Jupiter.y - Sun.y)) / norm(Z[0], Z[1]));
            }

            // Сатурн
            if (Saturn.x >= Sun.x) {
                Saturn.phis = acos(dot((Saturn.x - Sun.x), (Saturn.y - Sun.y), Z) / norm((Saturn.x - Sun.x), (Saturn.y - Sun.y)) / norm(Z[0], Z[1]));
            }
            else {
                Saturn.phis = -acos(dot((Saturn.x - Sun.x), (Saturn.y - Sun.y), Z) / norm((Saturn.x - Sun.x), (Saturn.y - Sun.y)) / norm(Z[0], Z[1]));
            }

            // Уран
            if (Uran.x >= Sun.x) {
                Uran.phis = acos(dot((Uran.x - Sun.x), (Uran.y - Sun.y), Z) / norm((Uran.x - Sun.x), (Uran.y - Sun.y)) / norm(Z[0], Z[1]));
            }
            else {
                Uran.phis = -acos(dot((Uran.x - Sun.x), (Uran.y - Sun.y), Z) / norm((Uran.x - Sun.x), (Uran.y - Sun.y)) / norm(Z[0], Z[1]));
            }

            // Нептун
            if (Neptune.x >= Sun.x) {
                Neptune.phis = acos(dot((Neptune.x - Sun.x), (Neptune.y - Sun.y), Z) / norm((Neptune.x - Sun.x), (Neptune.y - Sun.y)) / norm(Z[0], Z[1]));
            }
            else {
                Neptune.phis = -acos(dot((Neptune.x - Sun.x), (Neptune.y - Sun.y), Z) / norm((Neptune.x - Sun.x), (Neptune.y - Sun.y)) / norm(Z[0], Z[1]));
            }


            // Углы относительно планеты //

            // РН
            if (Rocket.x >= Earth.x) {
                Rocket.phip = acos(dot((Rocket.x - Earth.x), (Rocket.y - Earth.y), Z) / norm((Rocket.x - Earth.x), (Rocket.y - Earth.y)) / norm(Z[0], Z[1]));
            }
            else {
                Rocket.phip = -acos(dot((Rocket.x - Earth.x), (Rocket.y - Earth.y), Z) / norm((Rocket.x - Earth.x), (Rocket.y - Earth.y)) / norm(Z[0], Z[1]));
            }

            // Луна
            if (Luna.x >= Earth.x) {
                Luna.phip = acos(dot((Luna.x - Earth.x), (Luna.y - Earth.y), Z) / norm((Luna.x - Earth.x), (Luna.y - Earth.y)) / norm(Z[0], Z[1]));
            }
            else {
                Luna.phip = -acos(dot((Luna.x - Earth.x), (Luna.y - Earth.y), Z) / norm((Luna.x - Earth.x), (Luna.y - Earth.y)) / norm(Z[0], Z[1]));
            }


            // Углы относительно Луны //

            // РН 
            if (Rocket.x >= Luna.x) {
                Rocket.phil = acos(dot((Rocket.x - Luna.x), (Rocket.y - Luna.y), Z) / norm((Rocket.x - Luna.x), (Rocket.y - Luna.y)) / norm(Z[0], Z[1]));
            }
            else {
                Rocket.phil = -acos(dot((Rocket.x - Luna.x), (Rocket.y - Luna.y), Z) / norm((Rocket.x - Luna.x), (Rocket.y - Luna.y)) / norm(Z[0], Z[1]));
            }


           // Тяга двигателей //

            if (norm((Rocket.x - Earth.x), (Rocket.y - Earth.y)) - Earth.R < 100000) {

                TVm = Tvm[1][0];

                for (int i = 0; i < 29; i++) {
                    if (norm((Rocket.x - Earth.x), (Rocket.y - Earth.y)) - Earth.R >= Tvm[0][i]) {

                        // Температура воздуха на данной высоте, К
                        TVm = Tvm[1][i];

                        // Давление воздуха на данной высоте, Па
                        Pv = Pvm * pow(e, (-Mv * Rocket.gp * (norm((Rocket.x - Earth.x), (Rocket.y - Earth.y)) - Earth.R) / R / TVm));

                        // Плотность воздуха на данной высоте, кг/м3
                        rv = Pv * Mv / R / TVm;

                        // Тяга двигателей на данной высоте, кН
                        Ts = Tpp - (Tpp - Tmm) * rv / rvm;

                        // Удельная тяга твигателей на данной высоте, м/с
                        Is = Ipp - (Ipp - Imm) * rv / rvm;

                        break;
                    }
                }
            }
            else {
                Ts = Tpp;
                Is = Ipp;
            }

            if (hg == 0) {

                // Ускорение РН от двигателей, м/с2
                ad = Ts / Rocket.M * 1000;
            }
            else {
                ad = 0;
                Ts = 0;
            }


            // Полные ускорения //

            // Меркурий
            Mercury.ax = -Mercury.gs * sin(Mercury.phis);
            Mercury.ay = -Mercury.gs * cos(Mercury.phis);

            // Венера
            Venus.ax = -Venus.gs * sin(Venus.phis);
            Venus.ay = -Venus.gs * cos(Venus.phis);

            // Земля
            Earth.ax = -Earth.gs * sin(Earth.phis);
            Earth.ay = -Earth.gs * cos(Earth.phis);

            // Луна
            Luna.ax = -Luna.gs * sin(Luna.phis) - Luna.gp * sin(Luna.phip);
            Luna.ay = -Luna.gs * cos(Luna.phis) - Luna.gp * cos(Luna.phip);

            // Марс
            Mars.ax = -Mars.gs * sin(Mars.phis);
            Mars.ay = -Mars.gs * cos(Mars.phis);

            // Юпитер
            Jupiter.ax = -Jupiter.gs * sin(Jupiter.phis);
            Jupiter.ay = -Jupiter.gs * cos(Jupiter.phis);

            // Сатурн
            Saturn.ax = -Saturn.gs * sin(Saturn.phis);
            Saturn.ay = -Saturn.gs * cos(Saturn.phis);

            // Уран
            Uran.ax = -Uran.gs * sin(Uran.phis);
            Uran.ay = -Uran.gs * cos(Uran.phis);

            // Нептун
            Neptune.ax = -Neptune.gs * sin(Neptune.phis);
            Neptune.ay = -Neptune.gs * cos(Neptune.phis);


            // РН
            if (norm((Rocket.x - Earth.x), (Rocket.y - Earth.y)) - Earth.R < 20000) {
                Rocket.ax = (ad - Rocket.gp) * sin(Rocket.phip) - Rocket.gs * sin(Rocket.phis) - Rocket.gl * sin(Rocket.phil);
                Rocket.ay = (ad - Rocket.gp) * cos(Rocket.phip) - Rocket.gs * cos(Rocket.phis) - Rocket.gl * cos(Rocket.phil);
            }
            else if (t < t1) {

                    alpha = pi / 2 * (norm((Rocket.x - Earth.x), (Rocket.y - Earth.y)) - Earth.R) / 180000;
                    alpha = Rocket.phip + clamp(alpha, 0, pi / 2);

                    Rocket.ax = ad * sin(alpha) - Rocket.gs * sin(Rocket.phis) - Rocket.gp * sin(Rocket.phip) - Rocket.gl * sin(Rocket.phil);
                    Rocket.ay = ad * cos(alpha) - Rocket.gs * cos(Rocket.phis) - Rocket.gp * cos(Rocket.phip) - Rocket.gl * cos(Rocket.phil);
            }
            else if (t >= t1 && t < t2) {

                if (Rocket.y - Earth.y >= 0)
                    alpha = acos((Rocket.Vy - Earth.Vy) / norm((Rocket.Vx - Earth.Vx), (Rocket.Vy - Earth.Vy)));
                else
                    alpha = -acos((Rocket.Vy - Earth.Vy) / norm((Rocket.Vx - Earth.Vx), (Rocket.Vy - Earth.Vy)));

                Rocket.ax = ad * sin(alpha) - Rocket.gs * sin(Rocket.phis) - Rocket.gp * sin(Rocket.phip) - Rocket.gl * sin(Rocket.phil);
                Rocket.ay = ad * cos(alpha) - Rocket.gs * cos(Rocket.phis) - Rocket.gp * cos(Rocket.phip) - Rocket.gl * cos(Rocket.phil);
            }
            else if (t >= t2 && t < t3) {

                if (Rocket.y - Luna.y >= 0)
                    alpha = pi + acos((Rocket.Vy - Luna.Vy) / norm((Rocket.Vx - Luna.Vx), (Rocket.Vy - Luna.Vy)));
                else
                    alpha = pi - acos((Rocket.Vy - Luna.Vy) / norm((Rocket.Vx - Luna.Vx), (Rocket.Vy - Luna.Vy)));

                Rocket.ax = ad * sin(alpha) - Rocket.gs * sin(Rocket.phis) - Rocket.gp * sin(Rocket.phip) - Rocket.gl * sin(Rocket.phil);
                Rocket.ay = ad * cos(alpha) - Rocket.gs * cos(Rocket.phis) - Rocket.gp * cos(Rocket.phip) - Rocket.gl * cos(Rocket.phil);
            }
            else if (t >= t3 && !st) {

                alpha = -pi / 2 + Rocket.phil;

                if (t < t4) {
                    Rocket.ax = ad * sin(alpha) - Rocket.gs * sin(Rocket.phis) - Rocket.gp * sin(Rocket.phip) - Rocket.gl * sin(Rocket.phil);
                    Rocket.ay = ad * cos(alpha) - Rocket.gs * cos(Rocket.phis) - Rocket.gp * cos(Rocket.phip) - Rocket.gl * cos(Rocket.phil);
                }
                else {
                    Rocket.ax = 5 * ad * sin(alpha) - Rocket.gs * sin(Rocket.phis) - Rocket.gp * sin(Rocket.phip) - Rocket.gl * sin(Rocket.phil);
                    Rocket.ay = 5 * ad * cos(alpha) - Rocket.gs * cos(Rocket.phis) - Rocket.gp * cos(Rocket.phip) - Rocket.gl * cos(Rocket.phil);
                }
            }

            else {

            alpha = Rocket.phil;

            Rocket.ax = ad * sin(alpha) - Rocket.gs * sin(Rocket.phis) - Rocket.gp * sin(Rocket.phip) - Rocket.gl * sin(Rocket.phil);
            Rocket.ay = ad * cos(alpha) - Rocket.gs * cos(Rocket.phis) - Rocket.gp * cos(Rocket.phip) - Rocket.gl * cos(Rocket.phil);
            }

            // Скорости //

            // РН
            Rocket.Vx += Rocket.ax * dt;
            Rocket.Vy += Rocket.ay * dt;

            // Меркурий
            Mercury.Vx += Mercury.ax * dt;
            Mercury.Vy += Mercury.ay * dt;

            // Венера
            Venus.Vx += Venus.ax * dt;
            Venus.Vy += Venus.ay * dt;

            // Земля
            Earth.Vx += Earth.ax * dt;
            Earth.Vy += Earth.ay * dt;

            // Луна
            Luna.Vx += Luna.ax * dt;
            Luna.Vy += Luna.ay * dt;

            // Марс
            Mars.Vx += Mars.ax * dt;
            Mars.Vy += Mars.ay * dt;

            // Юпитер
            Jupiter.Vx += Jupiter.ax * dt;
            Jupiter.Vy += Jupiter.ay * dt;

            // Сатурн
            Saturn.Vx += Saturn.ax * dt;
            Saturn.Vy += Saturn.ay * dt;

            // Уран
            Uran.Vx += Uran.ax * dt;
            Uran.Vy += Uran.ay * dt;

            // Нептун
            Neptune.Vx += Neptune.ax * dt;
            Neptune.Vy += Neptune.ay * dt;


            // Координаты //

            // РН
            Rocket.x += Rocket.Vx * dt;
            Rocket.y += Rocket.Vy * dt;

            // Меркурий
            Mercury.x += Mercury.Vx * dt;
            Mercury.y += Mercury.Vy * dt;

            // Венера
            Venus.x += Venus.Vx * dt;
            Venus.y += Venus.Vy * dt;

            // Земля
            Earth.x += Earth.Vx * dt;
            Earth.y += Earth.Vy * dt;

            // Луна
            Luna.x += Luna.Vx * dt;
            Luna.y += Luna.Vy * dt;

            // Марс
            Mars.x += Mars.Vx * dt;
            Mars.y += Mars.Vy * dt;

            // Юпитер
            Jupiter.x += Jupiter.Vx * dt;
            Jupiter.y += Jupiter.Vy * dt;

            // Сатурн
            Saturn.x += Saturn.Vx * dt;
            Saturn.y += Saturn.Vy * dt;

            // Уран
            Uran.x += Uran.Vx * dt;
            Uran.y += Uran.Vy * dt;

            // Нептун
            Neptune.x += Neptune.Vx * dt;
            Neptune.y += Neptune.Vy * dt;


            // Массовый расход топлива, кг/с
            Rocket.M -= Ts / Is * 1000 * dt;
            Mtt -= Ts / Is * 1000 * dt;


            // Сброс ступени, если кончилось топливо
            if (Mtt <= 0) {
                switch (s) {
                case 0:
                    Mtt = Zwei.Mt;
                    Tpp = Zwei.Tp;
                    Tmm = Zwei.Tm;
                    Ipp = Zwei.Ip;
                    Imm = Zwei.Im;
                    Rocket.M = Zwei.M + Drei.M + Rb.M + A.M;
                    break;
                case 1:
                    Mtt = Drei.Mt;
                    Tpp = Drei.Tp;
                    Tmm = Drei.Tm;
                    Ipp = Drei.Ip;
                    Imm = Drei.Im;
                    Rocket.M = Drei.M + Rb.M + A.M;
                    break;
                case 2:
                    Mtt = Rb.Mt;
                    Tpp = Rb.Tp;
                    Tmm = Rb.Tp;
                    Ipp = Rb.Ip;
                    Imm = Rb.Ip;
                    Rocket.M = Rb.M + A.M;
                    break;
                case 3:
                    Mtt = A.Mt;
                    Tpp = A.Tm;
                    Tmm = A.Tm;
                    Ipp = A.Ip;
                    Imm = A.Ip;
                    Rocket.M = A.M;
                    break;
                }
                s++;
            }

            // Получение нужной скорости на орбите Земли
            if (norm((Rocket.x - Earth.x), (Rocket.y - Earth.y)) - Earth.R >= 180000 && hg == 0 && t <= 3000) {

                hg = 1;

                Rocket.Vy = Earth.Vy - 7800.650602 * sin(Rocket.phip);
                Rocket.Vx = Earth.Vx + 7800.650602 * cos(Rocket.phip);
            }

            T++;
            
            // Расчёт первого импульса для полёта к Луне
            if (t == t1) {

                hg = 0;
                Vg1 = norm((Rocket.Vx - Earth.Vx), (Rocket.Vy - Earth.Vy)) * (pow((2 * ((norm((Rocket.x - Luna.x), (Rocket.y - Luna.y)) + Luna.R + pow(10, 5)) / (norm((Rocket.x - Earth.x), (Rocket.y - Earth.y)))) / (((norm((Rocket.x - Luna.x), (Rocket.y - Luna.y)) + Luna.R + pow(10, 5)) / (norm((Rocket.x - Earth.x), (Rocket.y - Earth.y)))) + 1)), 0.5) - 1);
            }

            // Реализация первого импульса для полёта к Луне
            if (t >= t1 && t < t2) {
                dV += ad * dt;
                if (dV >= Vg1 * 1.015) {
                    hg = 1;
                    dV = 0;
                }
            }

            // Расчёт второго импульса для выхода на орбиту Луны 100 км
            if (t == t2) {
                hg = 0;
                Vg2 = abs(pow((G * Luna.M / (norm((Rocket.x - Luna.x), (Rocket.y - Luna.y)))), 0.5) * (pow((2 * ((Luna.R + 100000) / (norm((Rocket.x - Luna.x), (Rocket.y - Luna.y)))) / ((Luna.R + 100000) / (norm((Rocket.x - Luna.x), (Rocket.y - Luna.y))) + 1)), 0.5) - 1));
                Vg2 += abs(norm((Rocket.Vx - Luna.Vx), (Rocket.Vy - Luna.Vy)) - pow((G * Luna.M / (norm((Rocket.x - Luna.x), (Rocket.y - Luna.y)))), 0.5));
            }

            // Реализация второго импульса для выхода на орбиту Луны 100 км
            if (t >= t2 && t < 345500) {
                dV += ad * dt;
                if (dV >= Vg2) {
                    hg = 1;
                    dV = 0;
                }
            }
            
            // Выход аппарата на орбиту Луны 100 км
            if (norm((Rocket.x - Luna.x), (Rocket.y - Luna.y)) - Luna.R <= 100000 && t <= 346000) {
                Rocket.Vy = Luna.Vy - 1607.80548 * sin(Rocket.phil);
                Rocket.Vx = Luna.Vx + 1607.80548 * cos(Rocket.phil);
            }
            
            // Расчёт третьего импульса для снижения низшей точки орбиты до 18 км
            if (t == t3) {
                hg = 0;
                Vg3 = abs(norm((Rocket.Vx - Luna.Vx), (Rocket.Vy - Luna.Vy)) * (pow((2 * ((Luna.R + 18000) / (norm((Rocket.x - Luna.x), (Rocket.y - Luna.y)))) / ((Luna.R + 18000) / (norm((Rocket.x - Luna.x), (Rocket.y - Luna.y))) + 1)), 0.5) - 1));
            }

            // Реализация третьего импульса для снижения низшей точки орбиты до 18 км
            if (t >= t3 && t < 349700) {
                dV += ad * dt;
                if (dV >= Vg3) {
                    hg = 1;
                    dV = 0;
                }
            }
 
            if (t >= t4) {

                if (t == t4) {
                    hg = 0;
                }
                if (norm((Rocket.x - Luna.x), (Rocket.y - Luna.y)) - Luna.R > 6100) {
                    if ((Rocket.Vx - Luna.Vx) * cos(Rocket.phil) + (Rocket.Vy - Luna.Vy) * sin(Rocket.phil) <= 1) {
                        st = 1;
                        hg = 1;
                    }
                }
                
                if (norm((Rocket.x - Luna.x), (Rocket.y - Luna.y)) - Luna.R <= 6100) {
                    if (abs((Rocket.Vx - Luna.Vx) * sin(Rocket.phil) + (Rocket.Vy - Luna.Vy) * cos(Rocket.phil)) >= 2)
                        hg = 0;
                    else
                        hg = 1;
                }
            }

            if (norm((Rocket.x - Luna.x), (Rocket.y - Luna.y)) - Luna.R <= 0) {
                Tv = 0;
                std::cout << t - t4 << std::endl;
            }

            // Отрисовка //

            if (T == 1) {

                // Меркурий
                //CircleShape Mer(5.f);
                CircleShape Mer(Mercury.R * k);
                Mer.setPosition(width / 2 + (Mercury.x - Sun.x - X - Mercury.R) * k + dMx, height / 2 + (Mercury.y - Sun.y - Y - Mercury.R) * k + dMy);
                //Mer.setPosition(width / 2 + (Mercury.x - Sun.x - X) * k - 2.5f + dMx, height / 2 + (Mercury.y - Sun.y - Y) * k - 2.5f + dMy);
                Mer.setFillColor(Color(120, 120, 120));
                window.draw(Mer);

                // Венера
                //CircleShape Ven(5.f);
                CircleShape Ven(Venus.R * k);
                Ven.setPosition(width / 2 + (Venus.x - Sun.x - X - Venus.R) * k + dMx, height / 2 + (Venus.y - Sun.y - Y - Venus.R) * k - 2.5f + dMy);
                //Ven.setPosition(width / 2 + (Venus.x - Sun.x - X) * k - 2.5f + dMx, height / 2 + (Venus.y - Sun.y - Y) * k - 2.5f + dMy);
                Ven.setFillColor(Color(255, 219, 139));
                window.draw(Ven);

                // Земля
                //CircleShape Ear(5.f);
                CircleShape Ear(Earth.R * k);
                Ear.setPosition(width / 2 + (Earth.x - Sun.x - X - Earth.R) * k + dMx, height / 2 + (Earth.y - Sun.y - Y - Earth.R) * k + dMy);
                //Ear.setPosition(width / 2 + (Earth.x - Sun.x - X) * k - 2.5f + dMx, height / 2 + (Earth.y - Sun.y - Y) * k - 2.5f + dMy);
                Ear.setFillColor(Color(0, 128, 255));
                window.draw(Ear);

                // Луна
                //CircleShape Lun(5.f);
                CircleShape Lun(Luna.R * k);
                Lun.setPosition(width / 2 + (Luna.x - Sun.x - X - Luna.R) * k + dMx, height / 2 + (Luna.y - Sun.y - Y - Luna.R) * k + dMy);
                //Lun.setPosition(width / 2 + (Luna.x - Sun.x - X) * k - 2.5f + dMx, height / 2 + (Luna.y - Sun.y - Y) * k - 2.5f + dMy);
                //Lun.setFillColor(Color::White);
                Lun.setFillColor(Color::Black);
                window.draw(Lun);

                // Марс
                //CircleShape Mar(5.f);
                CircleShape Mar(Mars.R * k);
                Mar.setPosition(width / 2 + (Mars.x - Sun.x - X - Mars.R) * k + dMx, height / 2 + (Mars.y - Sun.y - Y - Mars.R) * k + dMy);
                //Mar.setPosition(width / 2 + (Mars.x - Sun.x - X) * k - 2.5f + dMx, height / 2 + (Mars.y - Sun.y - Y) * k - 2.5f + dMy);
                Mar.setFillColor(Color::Red);
                window.draw(Mar);

                // Юпитер
                //CircleShape Jup(5.f);
                CircleShape Jup(Jupiter.R * k);
                Jup.setPosition(width / 2 + (Jupiter.x - Sun.x - X - Jupiter.R) * k + dMx, height / 2 + (Jupiter.y - Sun.y - Y - Jupiter.R) * k + dMy);
                //Jup.setPosition(width / 2 + (Jupiter.x - Sun.x - X) * k - 2.5f + dMx, height / 2 + (Jupiter.y - Sun.y - Y) * k - 2.5f + dMy);
                Jup.setFillColor(Color(245, 245, 220));
                window.draw(Jup);

                // Сатурн
                //CircleShape Sat(5.f);
                CircleShape Sat(Saturn.R * k);
                Sat.setPosition(width / 2 + (Saturn.x - Sun.x - X - Saturn.R) * k + dMx, height / 2 + (Saturn.y - Sun.y - Y - Saturn.R) * k + dMy);
                //Sat.setPosition(width / 2 + (Saturn.x - Sun.x - X) * k - 2.5f + dMx, height / 2 + (Saturn.y - Sun.y - Y) * k - 2.5f + dMy);
                Sat.setFillColor(Color(166, 166, 0));
                window.draw(Sat);

                // Уран
                //CircleShape Ur(5.f);
                CircleShape Ur(Uran.R * k);
                Ur.setPosition(width / 2 + (Uran.x - Sun.x - X - Uran.R) * k + dMx, height / 2 + (Uran.y - Sun.y - Y - Uran.R) * k + dMy);
                //Ur.setPosition(width / 2 + (Uran.x - Sun.x - X) * k - 2.5f + dMx, height / 2 + (Uran.y - Sun.y - Y) * k - 2.5f + dMy);
                Ur.setFillColor(Color(0, 128, 128));
                window.draw(Ur);

                // Нептун
                //CircleShape Nep(5.f);
                CircleShape Nep(Neptune.R * k);
                Nep.setPosition(width / 2 + (Neptune.x - Sun.x - X - Neptune.R) * k + dMx, height / 2 + (Neptune.y - Sun.y - Y - Neptune.R) * k + dMy);
                //Nep.setPosition(width / 2 + (Neptune.x - Sun.x - X) * k - 2.5f + dMx, height / 2 + (Neptune.y - Sun.y - Y) * k - 2.5f + dMy);
                Nep.setFillColor(Color(70, 130, 180));
                window.draw(Nep);

                // РН
                //RectangleShape Roc(Vector2f(3 * pow(10, 5) * k, pow(10, 6) * k));
                CircleShape Roc(5.f);
                //Roc.setPosition(width / 2 + (Rocket.x - Sun.x - 1.5 * pow(10, 5) / 2 - X) * k + dMx, height / 2 + (Rocket.y - Sun.y - Y) * k + dMy);
                Roc.setPosition(width / 2 + (Rocket.x - Sun.x - X) * k - 2.5f + dMx, height / 2 + (Rocket.y - Sun.y - Y) * k - 2.5f + dMy);
                Roc.setFillColor(Color::Green);
                //Roc.setRotation(-alpha * 180 / pi);
                window.draw(Roc);

            }
        }

        if (T == 0) {
            
            // Меркурий
            //CircleShape Mer(5.f);
            CircleShape Mer(Mercury.R* k);
            Mer.setPosition(width / 2 + (Mercury.x - Sun.x - X - Mercury.R) * k + dMx, height / 2 + (Mercury.y - Sun.y - Y - Mercury.R) * k + dMy);
            //Mer.setPosition(width / 2 + (Mercury.x - Sun.x - X) * k - 2.5f + dMx, height / 2 + (Mercury.y - Sun.y - Y) * k - 2.5f + dMy);
            Mer.setFillColor(Color(120, 120, 120));
            window.draw(Mer);

            // Венера
            //CircleShape Ven(5.f);
            CircleShape Ven(Venus.R* k);
            Ven.setPosition(width / 2 + (Venus.x - Sun.x - X - Venus.R) * k + dMx, height / 2 + (Venus.y - Sun.y - Y - Venus.R) * k - 2.5f + dMy);
            //Ven.setPosition(width / 2 + (Venus.x - Sun.x - X) * k - 2.5f + dMx, height / 2 + (Venus.y - Sun.y - Y) * k - 2.5f + dMy);
            Ven.setFillColor(Color(255, 219, 139));
            window.draw(Ven);

            // Земля
            //CircleShape Ear(5.f);
            CircleShape Ear(Earth.R* k);
            Ear.setPosition(width / 2 + (Earth.x - Sun.x - X - Earth.R) * k + dMx, height / 2 + (Earth.y - Sun.y - Y - Earth.R) * k + dMy);
            //Ear.setPosition(width / 2 + (Earth.x - Sun.x - X) * k - 2.5f + dMx, height / 2 + (Earth.y - Sun.y - Y) * k - 2.5f + dMy);
            Ear.setFillColor(Color(0, 128, 255));
            window.draw(Ear);

            // Луна
            //CircleShape Lun(5.f);
            CircleShape Lun(Luna.R* k);
            Lun.setPosition(width / 2 + (Luna.x - Sun.x - X - Luna.R) * k + dMx, height / 2 + (Luna.y - Sun.y - Y - Luna.R) * k + dMy);
            //Lun.setPosition(width / 2 + (Luna.x - Sun.x - X) * k - 2.5f + dMx, height / 2 + (Luna.y - Sun.y - Y) * k - 2.5f + dMy);
            //Lun.setFillColor(Color::White);
            Lun.setFillColor(Color::Black);
            window.draw(Lun);

            // Марс
            //CircleShape Mar(5.f);
            CircleShape Mar(Mars.R* k);
            Mar.setPosition(width / 2 + (Mars.x - Sun.x - X - Mars.R) * k + dMx, height / 2 + (Mars.y - Sun.y - Y - Mars.R) * k + dMy);
            //Mar.setPosition(width / 2 + (Mars.x - Sun.x - X) * k - 2.5f + dMx, height / 2 + (Mars.y - Sun.y - Y) * k - 2.5f + dMy);
            Mar.setFillColor(Color::Red);
            window.draw(Mar);

            // Юпитер
            //CircleShape Jup(5.f);
            CircleShape Jup(Jupiter.R* k);
            Jup.setPosition(width / 2 + (Jupiter.x - Sun.x - X - Jupiter.R) * k + dMx, height / 2 + (Jupiter.y - Sun.y - Y - Jupiter.R) * k + dMy);
            //Jup.setPosition(width / 2 + (Jupiter.x - Sun.x - X) * k - 2.5f + dMx, height / 2 + (Jupiter.y - Sun.y - Y) * k - 2.5f + dMy);
            Jup.setFillColor(Color(245, 245, 220));
            window.draw(Jup);

            // Сатурн
            //CircleShape Sat(5.f);
            CircleShape Sat(Saturn.R* k);
            Sat.setPosition(width / 2 + (Saturn.x - Sun.x - X - Saturn.R) * k + dMx, height / 2 + (Saturn.y - Sun.y - Y - Saturn.R) * k + dMy);
            //Sat.setPosition(width / 2 + (Saturn.x - Sun.x - X) * k - 2.5f + dMx, height / 2 + (Saturn.y - Sun.y - Y) * k - 2.5f + dMy);
            Sat.setFillColor(Color(166, 166, 0));
            window.draw(Sat);

            // Уран
            //CircleShape Ur(5.f);
            CircleShape Ur(Uran.R* k);
            Ur.setPosition(width / 2 + (Uran.x - Sun.x - X - Uran.R) * k + dMx, height / 2 + (Uran.y - Sun.y - Y - Uran.R) * k + dMy);
            //Ur.setPosition(width / 2 + (Uran.x - Sun.x - X) * k - 2.5f + dMx, height / 2 + (Uran.y - Sun.y - Y) * k - 2.5f + dMy);
            Ur.setFillColor(Color(0, 128, 128));
            window.draw(Ur);

            // Нептун
            //CircleShape Nep(5.f);
            CircleShape Nep(Neptune.R* k);
            Nep.setPosition(width / 2 + (Neptune.x - Sun.x - X - Neptune.R) * k + dMx, height / 2 + (Neptune.y - Sun.y - Y - Neptune.R) * k + dMy);
            //Nep.setPosition(width / 2 + (Neptune.x - Sun.x - X) * k - 2.5f + dMx, height / 2 + (Neptune.y - Sun.y - Y) * k - 2.5f + dMy);
            Nep.setFillColor(Color(70, 130, 180));
            window.draw(Nep);

            // РН
            //RectangleShape Roc(Vector2f(3 * pow(10, 5) * k, pow(10, 6) * k));
            CircleShape Roc(5.f);
            //Roc.setPosition(width / 2 + (Rocket.x - Sun.x - 1.5 * pow(10, 5) / 2 - X) * k + dMx, height / 2 + (Rocket.y - Sun.y - Y) * k + dMy);
            Roc.setPosition(width / 2 + (Rocket.x - Sun.x - X) * k - 2.5f + dMx, height / 2 + (Rocket.y - Sun.y - Y) * k - 2.5f + dMy);
            Roc.setFillColor(Color::Green);
            //Roc.setRotation(-alpha * 180 / pi);
            window.draw(Roc);

        }

        window.display();
    }
}

// Функция нормализации вектора
double norm(double x, double y) {
    return sqrt(pow(x, 2) + pow(y, 2));
}

// Функция перемножения векторов
double dot(double x, double y, double Z[]) {
    return x * Z[0] + y * Z[1];
}

// Функция определения нахождения переменной в границах
double clamp(double value, double min, double max) {
    return fmax(fmin(value, max), min);
}