#include "n_body_tester.h"


void NBodyTester::TestCompare()
{
    std::vector<Body> bodies;

    bodies = GenerateBodies(body_count_ > -1 ? body_count_ : kBodyCountDefault);
    Timer serial_timer;
    Timer parallel_timer;
    SpaceTime serial;
    SpaceTime parallel;

    serial.SetBodies(bodies);
    parallel.SetBodies(bodies);

    int time = time_moment_ > -1 ? time_moment_ : kTimeMomentDefault;

    serial_timer.Start();
    serial.CalcTimeSerial(time);
    serial_timer.End();

    parallel_timer.Start();
    parallel.CalcTimeParallel(time);
    parallel_timer.End();

    long parallel_time = parallel_timer.GetNanosec();
    long serial_time = serial_timer.GetNanosec();
    long difference = serial_timer.GetNanosec() - parallel_timer.GetNanosec();
    double boost = 100 * (double)std::max(parallel_time, serial_time) /
                       (double)std::min(parallel_time, serial_time) -
                   100;

    std::cout << bodies.size() << "-тел симуляция\nМомент времени: " << time
              << std::endl;

    std::cout << "Параллельная версия : " << (double)parallel_time / 1E9 << " s / "
              << parallel_time << " ns\n"
              << "Последовательная версия  : " << (double)serial_time / 1E9 << " s / "
              << serial_time << " ns\n\n";

    std::cout << (difference > 0 ? "Параллельная" : "Последовательная")
              << " версия быстрее на "
              << (difference > 0 ? difference : -difference) << "ns\n"
              << "Прирост: " << boost << "%\n";

    bool error = false;
    SaveState(bodies, "input.txt");
    for (size_t i = 0; i < serial.GetBodies().size(); i++)
    {
        if (serial.GetBodies()[i].GetPosition() !=
            parallel.GetBodies()[i].GetPosition())
        {
            error = true;
            break;
        }
    }
    if (error)
    {
        std::cout << "Ошибка! Данные не совпадают!\n";
    }
    else {
        SaveState(serial.GetBodies(), "output_serial.txt");
        SaveState(parallel.GetBodies(), "output_parallel.txt");
    }

}

std::vector<Body> NBodyTester::GenerateBodies(unsigned num)
{
    const double kMaxMass = 100000 * 7.35E22;
    const double kMinMass = 7.35E22;

    const double kMinRadius = 1;
    const double kMaxRadius = 25;

    const double kMaxPosition = 1000000;
    const double kMinPosition = -1000000;

    const double kMaxVelocity = 1000;
    const double kMinVelocity = 1;

    std::vector<Body> bodies;

    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_real_distribution<double> vel_vec_distr(-1, 1);
    std::uniform_real_distribution<double> velocity_distr(kMinVelocity,
                                                          kMaxVelocity);
    std::uniform_real_distribution<double> mass_distr(kMinMass, kMaxMass);
    std::uniform_real_distribution<double> radius_distr(kMinRadius, kMaxRadius);
    std::uniform_real_distribution<double> position_distr(kMinPosition,
                                                          kMaxPosition);
    for (unsigned i = 0; i < num; i++)
    {
        double mass = mass_distr(eng);
        double radius = radius_distr(eng);

        double x = position_distr(eng);
        double y = position_distr(eng);
        double z = position_distr(eng);

        Vector3D position = {x, y, z};

        Vector3D velocity = {vel_vec_distr(eng), vel_vec_distr(eng),
                             vel_vec_distr(eng)};
        velocity = velocity * velocity_distr(eng);

        Body body(position, velocity, radius, mass);
        bodies.push_back(body);
    }
    return bodies;
}

void NBodyTester::SaveState(const std::vector<Body>& state,
                            std::string file_name) {
  std::ofstream out(file_name);

  out << "Координаты [x, y, z], скорость [x, y, z], масса, радиус\n";

  for (Body body : state) {
    Vector3D pos = body.GetPosition();
    Vector3D vel = body.GetVelocity();
    double mass = body.GetMass();
    double r = body.GetRadius();

    out << pos.x << ' ' << pos.y << ' ' << pos.z << ' ' << vel.x << ' ' << vel.y
        << ' ' << vel.z << ' ' << mass << ' ' << r << std::endl;
  }

  out.close();
}



