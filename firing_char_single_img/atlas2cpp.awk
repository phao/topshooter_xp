BEGIN {
  enum_body = "enum class AtlasPieceId {\n";
  atlas_def = "const AtlasPiece atlas_pieces[] = {\n";
}

{
  enum_body = enum_body "  " toupper($1) ",\n";
  atlas_def = atlas_def "  {" $2 ", " $3 ", " $4 ", " $5 ", " \
              "AtlasPieceId::" toupper($1) "},\n"
}

END {
  enum_body = enum_body "  NUM_ATLAS_PIECES\n};";
  atlas_def = atlas_def "};";

  print "namespace game {\n";

  print enum_body "\n";

  print "struct AtlasPiece {\n" \
        "  int x, y, w, h;\n" \
        "  AtlasPieceId id;\n" \
        "};\n"

  print atlas_def "\n";

  print "}\n";
}
