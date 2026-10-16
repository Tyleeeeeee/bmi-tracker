set pagination off
set confirm off
set charset UTF-8

break bmi::Classroom::add

break bmi::classify if value >= 24

catch throw bmi::InputError

info breakpoints
