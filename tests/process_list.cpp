// Copyright (C) 2010-2014 Joshua Boyce.
// See the file COPYING for copying permission.

#include <hadesmem/process_list.hpp>
#include <hadesmem/process_list.hpp>

#include <utility>

#include <hadesmem/detail/warning_disable_prefix.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <hadesmem/detail/warning_disable_suffix.hpp>

#include <hadesmem/config.hpp>
#include <hadesmem/process_entry.hpp>

void TestProcessList()
{
  using ProcessListIterCat =
    std::iterator_traits<hadesmem::ProcessList::iterator>::iterator_category;
  HADESMEM_DETAIL_STATIC_ASSERT(
    std::is_base_of<std::input_iterator_tag, ProcessListIterCat>::value);
  using ProcessListConstIterCat = std::iterator_traits<
    hadesmem::ProcessList::const_iterator>::iterator_category;
  HADESMEM_DETAIL_STATIC_ASSERT(
    std::is_base_of<std::input_iterator_tag, ProcessListConstIterCat>::value);

  hadesmem::ProcessList const process_list_1;
  hadesmem::ProcessList process_list_2(process_list_1);
  hadesmem::ProcessList process_list_3(std::move(process_list_2));
  process_list_2 = std::move(process_list_3);
  BOOST_TEST(std::begin(process_list_2) != std::end(process_list_2));

  auto iter = std::begin(process_list_1);
  BOOST_TEST(iter != std::end(process_list_1));
  BOOST_TEST(++iter != std::end(process_list_1));
  BOOST_TEST_NE(iter->GetId(), 0U);
  DWORD const second_id = iter->GetId();
  BOOST_TEST(++iter != std::end(process_list_1));
  DWORD const third_id = iter->GetId();
  BOOST_TEST_NE(second_id, third_id);
}

void TestProcessListAlgorithm()
{
  hadesmem::ProcessList const process_list_1;

  for (auto const& entry : process_list_1)
  {
    BOOST_TEST(!entry.GetName().empty());
  }

  auto const this_iter = std::find_if(std::begin(process_list_1),
                                      std::end(process_list_1),
                                      [](hadesmem::ProcessEntry const& entry)
                                      {
    return entry.GetId() == ::GetCurrentProcessId();
  });
  BOOST_TEST(this_iter != std::end(process_list_1));
}

int main()
{
  TestProcessList();
  TestProcessListAlgorithm();
  return boost::report_errors();
}
