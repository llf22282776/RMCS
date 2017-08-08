#include "group_feedback.hpp"

namespace hebi {

GroupFeedback::GroupFeedback(int number_of_modules)
 : internal_(hebiGroupFeedbackCreate(number_of_modules)),
   manage_pointer_lifetime_(true),
   number_of_modules_(number_of_modules)
{
  // TODO: throw exception for NULL internal ptr?
  for (int i = 0; i < number_of_modules_; i++)
    feedbacks_.emplace_back(hebiGroupFeedbackGetModuleFeedback(internal_, i));
}

GroupFeedback::GroupFeedback(HebiGroupFeedbackPtr const group_feedback)
 : internal_(group_feedback),
   manage_pointer_lifetime_(false),
   number_of_modules_(hebiGroupFeedbackGetNumModules(group_feedback))
{
  // TODO: exception on invalid group_feedback?
  for (int i = 0; i < number_of_modules_; i++)
    feedbacks_.emplace_back(hebiGroupFeedbackGetModuleFeedback(internal_, i));
}

GroupFeedback::~GroupFeedback() noexcept
{
  if (manage_pointer_lifetime_ && internal_ != nullptr)
    hebiGroupFeedbackRelease(internal_);
}

int GroupFeedback::size() const
{
  return number_of_modules_;
}

const Feedback& GroupFeedback::operator[](int index) const
{
  return feedbacks_[index];
}

Eigen::VectorXd GroupFeedback::getPosition() const
{
  Eigen::VectorXd res(number_of_modules_);
  for (int i = 0; i < number_of_modules_; ++i)
  {
    auto& fbk = feedbacks_[i].actuator().position();
    res[i] = (fbk) ? fbk.get() : std::numeric_limits<float>::quiet_NaN();
  }
  return res;
}
Eigen::VectorXd GroupFeedback::getVelocity() const
{
  Eigen::VectorXd res(number_of_modules_);
  for (int i = 0; i < number_of_modules_; ++i)
  {
    auto& fbk = feedbacks_[i].actuator().velocity();
    res[i] = (fbk) ? fbk.get() : std::numeric_limits<float>::quiet_NaN();
  }
  return res;
}
Eigen::VectorXd GroupFeedback::getTorque() const
{
  Eigen::VectorXd res(number_of_modules_);
  for (int i = 0; i < number_of_modules_; ++i)
  {
    auto& fbk = feedbacks_[i].actuator().torque();
    res[i] = (fbk) ? fbk.get() : std::numeric_limits<float>::quiet_NaN();
  }
  return res;
}

Eigen::VectorXd GroupFeedback::getPositionCommand() const
{
  Eigen::VectorXd res(number_of_modules_);
  for (int i = 0; i < number_of_modules_; ++i)
  {
    auto& cmd = feedbacks_[i].actuator().positionCommand();
    res[i] = (cmd) ? cmd.get() : std::numeric_limits<float>::quiet_NaN();
  }
  return res;
}
Eigen::VectorXd GroupFeedback::getVelocityCommand() const
{
  Eigen::VectorXd res(number_of_modules_);
  for (int i = 0; i < number_of_modules_; ++i)
  {
    auto& cmd = feedbacks_[i].actuator().velocityCommand();
    res[i] = (cmd) ? cmd.get() : std::numeric_limits<float>::quiet_NaN();
  }
  return res;
}
Eigen::VectorXd GroupFeedback::getTorqueCommand() const
{
  Eigen::VectorXd res(number_of_modules_);
  for (int i = 0; i < number_of_modules_; ++i)
  {
    auto& cmd = feedbacks_[i].actuator().torqueCommand();
    res[i] = (cmd) ? cmd.get() : std::numeric_limits<float>::quiet_NaN();
  }
  return res;
}

} // namespace hebi
