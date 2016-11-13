#include <vector>
#include <iostream>
#include <boost/numeric/odeint.hpp>
#include "gnuplot_i.hpp"

typedef std::vector<double> state_type;
typedef boost::numeric::odeint::runge_kutta_cash_karp54<state_type> stepper_type;

void right_part(const state_type &x, state_type &dxdt, const double t) {
	dxdt[0] = x[1];
	dxdt[1] = t * x[0];
}

class push_back_state_and_time {
	private:
    	std::vector<double> &m_xstates, &m_ystates;
    	std::vector<double> &m_times;

    public:
    	push_back_state_and_time(std::vector<double> &x_vec, std::vector<double> &y_vec, std::vector<double> &times):
    		m_xstates(x_vec), m_ystates(y_vec), m_times(times) {}

    	void operator()(const state_type &x, double t) {
        	m_xstates.push_back(x[0]);
        	m_ystates.push_back(x[1]);
        	m_times.push_back(t);
    	}
};

void wait_for_key() {
	std::cout << std::endl << "Press ENTER to continue..." << std::endl;

    std::cin.clear();
    std::cin.ignore(std::cin.rdbuf()->in_avail());
    std::cin.get();
}

int main() {
	std::vector<double> x, y, times;

	state_type x0 = {1.0, 2.0};
    std::vector<double> t = {0.0, 0.5, 1.0, 1.1, 1.2, 1.3, 1.5, 1.7, 2.5};

	int steps = boost::numeric::odeint::integrate_times(make_controlled(1E-12, 1E-12, stepper_type()),
		right_part, x0, t, 0.01, push_back_state_and_time(x, y, times));

	std::cout << steps << std::endl;
	for (int i = 0; i < times.size(); i++)
		std::cout << "t = " << times[i] << "   " << "x = [" << x[i] << ", " << y[i] << "]" << std::endl;

	try {
		Gnuplot gp("plot");
		gp.set_style("lines");
		gp.plot_xyz(times, x, y, "ODE solution");
		wait_for_key();
	} catch (GnuplotException gpe) {
		std::cout << gpe.what() << std::endl;
	}
}
