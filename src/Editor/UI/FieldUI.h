#pragma once

namespace batap
{
struct FieldType;

// Fills FieldType::drawUI for the builtin field types. Call once at App init.
void installFieldUI();

// drawUI for a field type known only by name — fields imported from a game
// DLL point at slots living in the DLL. Returns false for an unknown name.
bool installFieldUIFor(FieldType& type);
}  // namespace batap
