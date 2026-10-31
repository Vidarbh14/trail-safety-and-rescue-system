// =============================================================================
//  SECTION D  -  USER INTERFACE, PROGRESS BAR & CLI MENU
//  Author: Rajat Pant (Roll No. ML5/45)
// =============================================================================

#ifndef UI_HPP
#define UI_HPP

#include <string>

void renderProgressBar(int percentage, const std::string& actionLabel = "Traversing", int width = 28);
void clearCin();
bool readPositiveDouble(double& value);
void printMenu();

#endif // UI_HPP
