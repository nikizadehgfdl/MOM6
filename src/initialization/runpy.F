!Initially copied from https://github.com/wangsl/python-embedding
!
      Subroutine RunPy(PyScript, PyFunction, X, Nx, Y, Ny)
      Implicit None
      Character*(*) PyScript, PyFunction
      Real*8 X(*), Y(*)
      Integer Nx, Ny
      
      Integer I, Len, LinEnd
      External LinEnd
      Integer LenPyScript, LenPyFunction
      
      LenPyScript = LinEnd(PyScript)
      If(PyScript(LenPyScript-2:LenPyScript) .eq. '.py')
     $     LenPyScript = LenPyScript-3
      
      LenPyFunction = LinEnd(PyFunction)
      
      Call Flush(6)
      
      Call PyTest(PyScript, LenPyScript,
     $     PyFunction, LenPyFunction,
     $     X, Nx, Y, Ny)
      
      Return
      End

      integer function linend(cline)
      implicit real*8(a-h,o-z)
c
c     function which returns the length of a character string,
c     excluding trailing blanks, tabs and nulls.
c
      character*(*) cline, blank
      character*1 null, tab
      parameter (blank=' ')
c
      null = char(0)
      tab = char(9)
      do 5 i = len(cline), 1, -1
          if(cline(i:i).ne.blank.and.
     $    cline(i:i).ne.null.and.
     $    cline(i:i).ne.tab) goto 10
    5     continue
      linend = 0
      return
   10 linend = i
      return
      end

