#include "lookup.hpp"
#include <algorithm> // For std::transform
#include <iterator> // For std::back_inserter on Windows.

namespace hebi {

Lookup::Lookup()
{
  lookup_ = hebiLookupCreate();
}

Lookup::~Lookup() noexcept
{
  hebiLookupRelease(lookup_);
}

void Lookup::printTable()
{
  hebiPrintLookupTable(lookup_);
}

std::unique_ptr<Group> Lookup::getGroupFromNames(const std::vector<std::string>& names, const std::vector<std::string>& families, long timeout_ms)
{
  std::unique_ptr<Group> ptr;
  std::vector<const char *> names_cstrs;
  std::vector<const char *> families_cstrs;
  names_cstrs.reserve(names.size());
  families_cstrs.reserve(families.size());

  std::transform(std::begin(names), std::end(names),
    std::back_inserter(names_cstrs), [] (const std::string& name) { return name.c_str(); });
  std::transform(std::begin(families), std::end(families),
    std::back_inserter(families_cstrs), [] (const std::string& family) { return family.c_str(); });

  HebiGroupPtr group = hebiCreateGroupFromNames(lookup_, names_cstrs.data(), names_cstrs.size(), families_cstrs.data(), families_cstrs.size(), timeout_ms);
  if (group != nullptr)
    ptr.reset(new Group(group));
  return ptr;
}

std::unique_ptr<Group> Lookup::getGroupFromMacs(const std::vector<MacAddress>& addresses, long timeout_ms)
{
  std::unique_ptr<Group> ptr;
  std::vector<HebiMacAddress> addresses_c;
  addresses_c.reserve(addresses.size());
  std::transform(std::begin(addresses), std::end(addresses),
    std::back_inserter(addresses_c), [] (const MacAddress& addr) { return addr.internal_; });
  HebiGroupPtr group = hebiCreateGroupFromMacs(lookup_, addresses_c.data(), addresses.size(), timeout_ms);
  if (group != nullptr)
    ptr.reset(new Group(group));
  return ptr;
}

std::unique_ptr<Group> Lookup::getGroupFromFamily(const std::string& family, long timeout_ms)
{
  std::unique_ptr<Group> ptr;
  HebiGroupPtr group = hebiCreateGroupFromFamily(lookup_, family.c_str(), timeout_ms);
  if (group != nullptr)
    ptr.reset(new Group(group));
  return ptr;
}

std::unique_ptr<Group> Lookup::getConnectedGroupFromName(const std::string& name, const std::string& family_name, long timeout_ms)
{
  std::unique_ptr<Group> ptr;
  HebiGroupPtr group = hebiCreateConnectedGroupFromName(lookup_, name.c_str(), family_name.c_str(), timeout_ms);
  if (group != nullptr)
    ptr.reset(new Group(group));
  return ptr;
}

std::unique_ptr<Group> Lookup::getConnectedGroupFromMac(const MacAddress& address, long timeout_ms)
{
  std::unique_ptr<Group> ptr;
  HebiGroupPtr group = hebiCreateConnectedGroupFromMac(lookup_, &(address.internal_), timeout_ms);
  if (group != nullptr)
    ptr.reset(new Group(group));
  return ptr;
}

Lookup::EntryList::~EntryList() noexcept
{
  hebiLookupEntryListRelease(lookup_list_);
  lookup_list_ = nullptr;
}

Lookup::EntryList::Entry Lookup::EntryList::getEntry(int index)
{
  int required_size = hebiLookupEntryListGetName(lookup_list_, index, nullptr, 0);
  char* buffer = new char [required_size];
  hebiLookupEntryListGetName(lookup_list_, index, buffer, required_size);
  std::string name(buffer);
  delete[] buffer;

  required_size = hebiLookupEntryListGetFamily(lookup_list_, index, nullptr, 0);
  buffer = new char [required_size];
  hebiLookupEntryListGetFamily(lookup_list_, index, buffer, required_size);
  std::string family(buffer);
  delete[] buffer;

  HebiMacAddress mac_int = hebiLookupEntryListGetMacAddress(lookup_list_, index);
  MacAddress mac;
  mac.internal_ = mac_int;

  Entry e = { name, family, mac };
  return e;
}

int Lookup::EntryList::size()
{
  return hebiLookupEntryListGetNumberOfEntries(lookup_list_);
}

std::unique_ptr<Lookup::EntryList> Lookup::getEntryList()
{
  std::unique_ptr<Lookup::EntryList> ptr;
  auto entry_list = hebiLookupCreateLookupEntryList(lookup_);
  if (entry_list != nullptr)
    ptr.reset(new Lookup::EntryList(entry_list));
  return ptr;
}

} // namespace hebi
