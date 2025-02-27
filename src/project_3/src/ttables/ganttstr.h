#ifndef GANTTSTR_H
#define GANTTSTR_H


// This header is a VERY dirty way of setting the gantt environment options
// It would be nicer to move this into individual files and read them into memory


#define GANTT_HEADER "\\begin{ganttchart}[\n"\
                     "  hgrid,\n"\
                     "  vgrid,\n"\
                     "  x unit=.5cm,\n"\
                     "  y unit chart=%.2fcm,\n"\
                     "  y unit title=.2cm,\n"\
                     "  title/.style={draw=none, fill=none},\n"\
                     "  title label font=\\tiny,\n"\
                     "  include title in canvas=false,\n"\
                     "  bar label font=\\small,\n"\
                     "  vrule/.style={line width=2pt, dotted},\n"\
                     "  vrule label font=\\tiny,\n"\
                     "  vrule offset=0,\n"\
                     "  expand chart=\\textwidth\n"\
                     "]"


#define GANTT_TAIL "\n\\end{ganttchart}\n\n"


#define ROW_LABEL "\\ganttbar[bar/.append style={draw=none, fill=none}]"


#define ROW_BREAK "\n\\\\\n\n"


#define BEGIN_FRAME "\n\\begin{frame}{Simulación del scheduler}\n\n"


#endif // GANTTSTR_H