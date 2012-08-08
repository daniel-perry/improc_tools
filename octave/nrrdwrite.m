function nrrdwrite(filename, X)
% NRRDWRITE writes out a barebones nrrd file.
% NRRDWRITE(FILENAME, X) writes image volume and 
% some barebones metadata to the file specified by
% FILENAME.
%
% Caveats:
% * did this fast one afternoon, trying to avoid
%   the pain of compiling the mex nrrd inteface.
% * this should get a basic nrrd file, but not
%   much beyond that.
% * if you want the real deal, I suggest compiling
%   the mex nrrd interface, for details see:
%   http://teem.sourceforge.net
%

% Copyright 2012 University of Utah

% minimalist header:
% NRRD0001
% # my first nrrd
% type: uchar
% dimension: 3
% sizes: 3 640 480
% encoding: raw
% endian: little
%

fid = fopen(filename, 'wb');
assert(fid > 0, 'Could not open file.');
cleaner = onCleanup(@() fclose(fid));

% figure out type:
datatype = '';
if (isa(X, 'double'))
  X = single(X); % what is wanted most of the time.
  datatype = 'float';
elseif (isa(X, 'single'))
  datatype = 'float';
elseif (isa(X,'int8') || isa(X,'char'))
  datatype = 'int8';
elseif (isa(X,'int16'))
  datatype = 'int16';
elseif (isa(X,'int32'))
  datatype = 'int32';
elseif (isa(X,'int64'))
  datatype = 'int64';
elseif (isa(X,'uint8') || isa(X,'uchar'))
  datatype = 'uchar';
elseif (isa(X,'uint16'))
  datatype = 'uint16';
elseif (isa(X,'uint32'))
  datatype = 'uint32';
elseif (isa(X,'uint64'))
  datatype = 'uint64';
else
  error(sprintf('Error: cant handle type: %s',class(X)));
end

datatype = sprintf('type: %s',datatype);

% sizes and dimensions and encoding
datasizes = size(X);
datadim = size(datasizes,2);

% reorder from MATLAB's order:
% todo: works for 3-d, verify this works for n-d...
tmp = datasizes(1);
datasizes(1) = datasizes(2);
datasizes(2) = tmp;
X = permute(X, [2 1 3]);

sizes = 'sizes:';
for (i = 1:datadim)
  sizes = sprintf( '%s %d', sizes, datasizes(i) );
end
dim = sprintf('dimension: %d',datadim);

encoding = 'encoding: raw';

% write header:
fprintf( fid, 'NRRD0001\n' );
fprintf( fid, '%s\n', datatype );
fprintf( fid, '%s\n', dim );
fprintf( fid, '%s\n', sizes );
fprintf( fid, '%s\n', encoding );
fprintf( fid, 'endian: little\n' );
fprintf( fid, '\n' ); % blank lines ends header, begins data section.

% write data:
fwrite( fid, X(:), class(X) );
