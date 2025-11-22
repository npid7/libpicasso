#include <pica.hpp>
#include <picasso/picasso.h>
#ifndef __3DS__
// f24 has:
//  - 1 sign bit
//  - 7 exponent bits
//  - 16 mantissa bits
uint32_t f32tof24(float f) {
  uint32_t i;
  memcpy(&i, &f, sizeof(f));

  uint32_t mantissa = (i << 9) >> 9;
  int32_t exponent = (i << 1) >> 24;
  uint32_t sign = (i << 0) >> 31;

  // Truncate mantissa
  mantissa >>= 7;

  // Re-bias exponent
  exponent = exponent - 127 + 63;
  if (exponent < 0) {
    // Underflow: flush to zero
    return sign << 23;
  } else if (exponent > 0x7F) {
    // Overflow: saturate to infinity
    return (sign << 23) | (0x7F << 16);
  }

  return (sign << 23) | (exponent << 16) | mantissa;
}
#endif

void BasicHandler(const char *top, const char *message) {
  std::cout << top << std::endl << message << std::endl;
}

static void (*EHND)(const char *top, const char *message) = BasicHandler;

namespace Pica {

void InstallErrorCallback(void (*ErrorHandler)(const char *top,
                                               const char *message)) {
  EHND = ErrorHandler;
}

std::vector<u8> AssembleCode(const std::string& vertex) {
  int rc = 0;
  rc = AssembleString(vertex.c_str(), "llc_npi");
  if (rc) {
    EHND("Error when Assembling Code", vertex.c_str());
  }

  rc = RelocateProduct();
  if (rc) {
    EHND("Error when Relocating Product", "0");
  }
  FileClass f("Dont Care", "wb");

  u32 progSize = g_outputBuf.size();
  u32 dvlpSize = 10 * 4 + progSize * 4 + g_opdescCount * 8;

  // Write DVLB header
  f.WriteWord(0x424C5644);       // DVLB
  f.WriteWord(g_totalDvleCount); // Number of DVLEs

  // Calculate and write DVLE offsets
  u32 curOff = 2 * 4 + g_totalDvleCount * 4 + dvlpSize;
  for (dvleTableIter dvle = g_dvleTable.begin(); dvle != g_dvleTable.end();
       ++dvle) {
    if (dvle->nodvle)
      continue;
    f.WriteWord(curOff);
    curOff += 16 * 4; // Header
    curOff += dvle->constantCount * 20;
    curOff += dvle->outputCount * 8;
    curOff += dvle->uniformCount * 8;
    curOff += dvle->symbolSize;
    curOff = (curOff + 3) & ~3; // Word alignment
  }

  // Write DVLP header
  f.WriteWord(0x504C5644);            // DVLP
  f.WriteWord(0);                     // version
  f.WriteWord(10 * 4);                // offset to shader binary blob
  f.WriteWord(progSize);              // size of shader binary blob
  f.WriteWord(10 * 4 + progSize * 4); // offset to opdesc table
  f.WriteWord(g_opdescCount);         // number of opdescs
  f.WriteWord(dvlpSize);              // offset to symtable (TODO)
  f.WriteWord(0);                     // ????
  f.WriteWord(0);                     // ????
  f.WriteWord(0);                     // ????

  // Write program
  for (outputBufIter it = g_outputBuf.begin(); it != g_outputBuf.end(); ++it)
    f.WriteWord(*it);

  // Write opdescs
  for (int i = 0; i < g_opdescCount; i++)
    f.WriteDword(g_opdescTable[i]);

  // Write DVLEs
  for (dvleTableIter dvle = g_dvleTable.begin(); dvle != g_dvleTable.end();
       ++dvle) {
    if (dvle->nodvle)
      continue;
    curOff = 16 * 4;

    f.WriteWord(0x454C5644);                // DVLE
    f.WriteHword(0x1002);                   // maybe version?
    f.WriteByte(dvle->isGeoShader ? 1 : 0); // Shader type
    f.WriteByte(dvle->isMerge ? 1 : 0);
    f.WriteWord(dvle->entryStart); // offset to main
    f.WriteWord(dvle->entryEnd);   // offset to end of main
    f.WriteHword(dvle->inputMask);
    f.WriteHword(dvle->outputMask);
    f.WriteByte(dvle->geoShaderType);
    f.WriteByte(dvle->geoShaderFixedStart);
    f.WriteByte(dvle->geoShaderVariableNum);
    f.WriteByte(dvle->geoShaderFixedNum);
    f.WriteWord(curOff);              // offset to constant table
    f.WriteWord(dvle->constantCount); // size of constant table
    curOff += dvle->constantCount * 5 * 4;
    f.WriteWord(curOff);            // offset to label table (TODO)
    f.WriteWord(0);                 // size of label table (TODO)
    f.WriteWord(curOff);            // offset to output table
    f.WriteWord(dvle->outputCount); // size of output table
    curOff += dvle->outputCount * 8;
    f.WriteWord(curOff);             // offset to uniform table
    f.WriteWord(dvle->uniformCount); // size of uniform table
    curOff += dvle->uniformCount * 8;
    f.WriteWord(curOff);           // offset to symbol table
    f.WriteWord(dvle->symbolSize); // size of symbol table

    // Sort uniforms by position
    std::sort(dvle->uniformTable, dvle->uniformTable + dvle->uniformCount);

    // Write constants
    for (int i = 0; i < dvle->constantCount; i++) {
      Constant &ct = dvle->constantTable[i];
      f.WriteHword(ct.type);
      if (ct.type == UTYPE_FVEC) {
        f.WriteHword(ct.regId - 0x20);
        for (int j = 0; j < 4; j++)
          f.WriteWord(f32tof24(ct.fparam[j]));
      } else if (ct.type == UTYPE_IVEC) {
        f.WriteHword(ct.regId - 0x80);
        for (int j = 0; j < 4; j++)
          f.WriteByte(ct.iparam[j]);
      } else if (ct.type == UTYPE_BOOL) {
        f.WriteHword(ct.regId - 0x88);
        f.WriteWord(ct.bparam ? 1 : 0);
      }
      if (ct.type != UTYPE_FVEC)
        for (int j = 0; j < 3; j++)
          f.WriteWord(0); // Padding
    }

    // Write outputs
    for (int i = 0; i < dvle->outputCount; i++)
      f.WriteDword(dvle->outputTable[i]);

    // Write uniforms
    size_t sp = 0;
    for (int i = 0; i < dvle->uniformCount; i++) {
      Uniform &u = dvle->uniformTable[i];
      size_t l = u.name.length() + 1;
      f.WriteWord(sp);
      sp += l;
      int pos = u.pos;
      if (pos >= 0x20)
        pos -= 0x10;
      f.WriteHword(pos);
      f.WriteHword(pos + u.size - 1);
    }

    // Write symbols
    for (int i = 0; i < dvle->uniformCount; i++) {
      std::string u(dvle->uniformTable[i].name);
      std::replace(u.begin(), u.end(), '$', '.');
      size_t l = u.length() + 1;
      f.WriteRaw(u.c_str(), l);
    }

    // Word alignment
    int pos = f.Tell();
    int pad = ((pos + 3) & ~3) - pos;
    for (int i = 0; i < pad; i++)
      f.WriteByte(0);
  }
  return std::vector<u8>(f.get_ptr()->str().c_str(), f.get_ptr()->str().c_str()+f.Tell());
}

std::vector<u8> AssembleFile(const std::string& file) {
  char *sourceCode = StringFromFile(file.c_str());
  if (!sourceCode) {
    EHND("error:", "cannot open input file!\n");
  }
  return AssembleCode(sourceCode);
}
} // namespace Pica