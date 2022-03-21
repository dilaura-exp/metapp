// metapp library
// 
// Copyright (C) 2022 Wang Qi (wqking)
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

namespace metapp {

inline MetaRepo * getMetaRepo()
{
	static MetaRepo repo;
	return &repo;
}

inline MetaRepo::MetaRepo()
{
	registerBuiltinTypes();
}

inline MetaRepo * MetaRepo::addRepo(const std::string & name)
{
	std::unique_ptr<MetaRepo> repo(new MetaRepo());
	MetaRepo * result = repo.get();
	repoMap[name] = std::move(repo);
	return result;
}

inline std::vector<std::string> MetaRepo::getRepoNameList() const
{
	return internal_::getMapKeys(repoMap);
}

inline void MetaRepo::registerBuiltinTypes()
{
	addType<void>();
	addType<bool>();
	addType<char>();
	addType<wchar_t>();
	addType<signed char>();
	addType<unsigned char>();
	addType<short>();
	addType<unsigned short>();
	addType<int>();
	addType<unsigned int>();
	addType<long>();
	addType<unsigned long>();
	addType<long long>();
	addType<unsigned long long>();
	addType<float>();
	addType<double>();
	addType<long double>();
	addType<std::string>();
	addType<std::wstring>();
}


} // namespace metapp
