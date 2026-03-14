#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include "AIEnvironment.h"

namespace py = pybind11;

PYBIND11_MODULE(nw_engine, m)
{
     py::class_<AIEnvironment>(m, "AIEnv")
         .def(py::init<>())
         .def("initContext", &AIEnvironment::initContext)
         .def("init", &AIEnvironment::Init)
         .def("reset", [](AIEnvironment &self)
              {
        StepResult res = self.Reset();
        return py::array_t<float>({64, 64, 4}, res.image_data.data()); })
         .def("step", [](AIEnvironment &self, std::vector<float> actions)
              {
        StepResult res = self.Step(actions);
        py::array_t<float> obs = py::array_t<float>({64, 64, 4}, res.image_data.data());
        return py::make_tuple(obs, res.reward, res.done); });
}