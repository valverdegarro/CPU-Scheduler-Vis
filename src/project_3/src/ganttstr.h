#ifndef GANTTSTR_H
#define GANTTSTR_H


// This header is a VERY dirty way of setting the gantt environment options
// It would be nicer to move this into individual files and read them into memory


// TODO: move this to the preamble in base.tex
#define GANTT_VLINE "\\newganttchartelement*{vline}{\n"\
                    "   vline/.style={\n"\
                    "   red, draw, thick, outer sep=0 pt, inner sep=0 pt,\n"\
                    "}, \n"\
                    "   vline right shift=-1,\n"\
                    "   vline top shift=0,\n"\
                    "   vline height=1,\n}\n\n"


#define GANTT_HEADER "\\begin{ganttchart}[\n"\
                     "  hgrid,\n"\
                     "  vgrid,\n"\
                     "  x unit=.5cm,\n"\
                     "  y unit chart=.5cm,\n"\
                     "  y unit title=.2cm,\n"\
                     "  title/.style={draw=none, fill=none},\n"\
                     "  title label font=\\tiny,\n"\
                     "  include title in canvas=false,\n"\
                     "  bar label font=\\small,\n"\
                     "  vrule/.style={line width=2pt, dotted},\n"\
                     "  vrule label font=\\tiny,\n"\
                     "  vrule offset=0,\n"\
                     "  expand chart=\\textwidth\n"\
                     "]{0}"


#define GANTT_TAIL "\n\\end{ganttchart}\n\n"


#define ROW_LABEL "\\ganttbar[bar/.append style={draw=none, fill=none}]"


#define ROW_BREAK "\n\\\\\n\n"


#endif // GANTTSTR_H