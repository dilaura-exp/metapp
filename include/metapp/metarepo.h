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

#ifndef METAPP_METAREPO_H_969872685611
#define METAPP_METAREPO_H_969872685611

#include "metapp/metatypes/metatypes.h"
#include "metapp/implement/internal/metarepobase_i.h"

namespace metapp {

class MetaRepo : public internal_::MetaRepoBase
{
public:
	MetaRepo();
	~MetaRepo() = default;

	MetaRepo(const MetaRepo &) = delete;
	MetaRepo(MetaRepo &&) = delete;

	MetaRepo * addRepo(const std::string & name);

	std::vector<std::string> getRepoNameList() const;

private:
	void registerBuiltinTypes();

private:
	std::map<std::string, std::unique_ptr<MetaRepo> > repoMap;
};

MetaRepo * getMetaRepo();

} // namespace metapp

#include "metapp/implement/metarepo_impl.h"

#endif