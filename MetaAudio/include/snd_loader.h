#pragma once
#include <iostream>

#include "alure/AL/alure2.h"

class GoldSrcFileFactory final : public alure::FileIOFactory {
public:
  alure::UniquePtr<std::istream> openFile(const alure::String &name) noexcept override;
};