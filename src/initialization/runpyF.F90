!Inspired by https://github.com/wangsl/python-embedding
!
module MOM_python_embedding
implicit none
public runPyF_1_3d
contains

subroutine runpyF_1_3d(pyScript, pyFunction, varname, x)
  character(len=*),                   intent(in)    :: pyScript, pyFunction,varname
  real, dimension(:,:,:),             intent(inout) :: x 
  integer :: n1,n2,n3, len_pyScript, len_pyFunction , len_varname   

  len_pyScript = Len_Trim(pyScript)
  if(pyScript(len_pyScript-2:len_pyScript) .eq. '.py') len_pyScript = len_pyScript-3   
  len_pyFunction = Len_Trim(pyFunction)   
  len_varname  = Len_Trim(varname)   

  n1 = size(x,1)
  n2 = size(x,2)       
  n3 = size(x,3)       
  call runpyc_1array3d(pyScript, len_pyScript, pyFunction, len_pyFunction,&
                       varname, len_varname,  x, n1,n2,n3)
      
end subroutine runpyF_1_3d

end module MOM_python_embedding
