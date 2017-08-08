#include "group_command.hpp"

namespace hebi {

GroupCommand::GroupCommand(int number_of_modules)
 : internal_(hebiGroupCommandCreate(number_of_modules)),
   manage_pointer_lifetime_(true),
   number_of_modules_(number_of_modules)
{
  // TODO: throw exception for NULL internal ptr?
  for (int i = 0; i < number_of_modules_; i++)
    commands_.emplace_back(hebiGroupCommandGetModuleCommand(internal_, i));
}

GroupCommand::~GroupCommand() noexcept
{
  if (manage_pointer_lifetime_ && internal_ != nullptr)
    hebiGroupCommandRelease(internal_);
}

int GroupCommand::size() const
{
  return number_of_modules_;
}

Command& GroupCommand::operator[](int index)
{
  return commands_[index];
}

void GroupCommand::setPosition(const Eigen::VectorXd& position)
{
  if (position.size() != number_of_modules_)
    return;
  for (int i = 0; i < number_of_modules_; ++i)
    commands_[i].actuator().position().set(position[i]);
}
void GroupCommand::setVelocity(const Eigen::VectorXd& velocity)
{
  if (velocity.size() != number_of_modules_)
    return;
  for (int i = 0; i < number_of_modules_; ++i)
    commands_[i].actuator().velocity().set(velocity[i]);
}
void GroupCommand::setTorque(const Eigen::VectorXd& torque)
{
  if (torque.size() != number_of_modules_)
    return;
  for (int i = 0; i < number_of_modules_; ++i)
    commands_[i].actuator().torque().set(torque[i]);
}

Eigen::VectorXd GroupCommand::getPosition() const
{
  Eigen::VectorXd res(number_of_modules_);
  for (int i = 0; i < number_of_modules_; ++i)
  {
    const auto& cmd = commands_[i].actuator().position();
    res[i] = (cmd) ? cmd.get() : std::numeric_limits<float>::quiet_NaN();
  }
  return res;
}
Eigen::VectorXd GroupCommand::getVelocity() const
{
  Eigen::VectorXd res(number_of_modules_);
  for (int i = 0; i < number_of_modules_; ++i)
  {
    const auto& cmd = commands_[i].actuator().velocity();
    res[i] = (cmd) ? cmd.get() : std::numeric_limits<float>::quiet_NaN();
  }
  return res;
}
Eigen::VectorXd GroupCommand::getTorque() const
{
  Eigen::VectorXd res(number_of_modules_);
  for (int i = 0; i < number_of_modules_; ++i)
  {
    const auto& cmd = commands_[i].actuator().torque();
    res[i] = (cmd) ? cmd.get() : std::numeric_limits<float>::quiet_NaN();
  }
  return res;
}

} // namespace hebi
