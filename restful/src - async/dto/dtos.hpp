#ifndef DTO_HPP
#define DTO_HPP

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO) ///< Begin DTO codegen section

class FixValueParaDto : public oatpp::DTO {
    DTO_INIT(FixValueParaDto, DTO /* extends */)
    DTO_FIELD(String, substation, "substation") = "";
    DTO_FIELD(Int32, area, "area") = 0;
    DTO_FIELD(Int32, round, "round") = 0;
};

class FixValueRequestDto : public oatpp::DTO {
    DTO_INIT(FixValueRequestDto, DTO /* extends */)
    DTO_FIELD(String, optype, "optype");
    DTO_FIELD(Object<FixValueParaDto>, para, "para");
};

#include OATPP_CODEGEN_END(DTO) ///< End DTO codegen section

#endif
