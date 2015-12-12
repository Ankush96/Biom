function varargout = color(varargin)

gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @color_OpeningFcn, ...
                   'gui_OutputFcn',  @color_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before color is made visible.
function color_OpeningFcn(hObject, eventdata, handles, varargin)
% Choose default command line output for color
handles.output = hObject;

[handles.files, labels] = dir_read('../Data/');
% Check to see if an empty list is loaded
if length(labels) == 0
    fprintf(' Change argumnet in dir_read inside color.m ');
end

% Store the paths to all the images
for i = 1:length(handles.files)
    handles.X{i} = char(handles.files(i));
end
% Load and save the source image in the handles.current_img
% All operations will be done on this image
handles.current_img = imread(handles.X{1});
handles.current_rgb = handles.current_img;
handles.current_hsv = handles.current_img;
handles.current_ycbcr = handles.current_img;

axes(handles.axes1);
imshow(handles.current_img);

axes(handles.axes2);
imshow(handles.current_hsv);

axes(handles.axes3);
imshow(handles.current_ycbcr);

axes(handles.axes4);
imshow(handles.current_rgb);
handles.R_min = 0;
handles.G_min = 0;
handles.B_min = 0;
handles.R_max = 255;
handles.G_max = 255;
handles.B_max = 255;
%------------------------------------------
handles.index = 1;
Cek(hObject, eventdata, handles);
% Update handles structure
guidata(hObject, handles);

% UIWAIT makes color wait for user response (see UIRESUME)
% uiwait(handles.figure1);

% --- Outputs from this function are returned to the command line.
function varargout = color_OutputFcn(hObject, eventdata, handles) 
% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes when figure1 is resized.
function figure1_ResizeFcn(hObject, eventdata, handles)

function pushbutton1_Callback(hObject, eventdata, handles)
handles.output = hObject;
handles.index = handles.index - 1;
axes(handles.axes1);
Cek(hObject, eventdata, handles);
handles.current_img = imread(handles.X{handles.index});
imshow(handles.current_img);
rgb_filter(hObject, eventdata, handles);
guidata(hObject, handles);

function pushbutton2_Callback(hObject, eventdata, handles)
handles.output = hObject;
handles.index = handles.index + 1;
axes(handles.axes1);
Cek(hObject, eventdata, handles);
handles.current_img = imread(handles.X{handles.index});
imshow(handles.current_img);
rgb_filter(hObject, eventdata, handles);
guidata(hObject, handles);

% --- Checks if RGB values are within the RGB thresholds in SLider and puts
%     mask accordingly
function rgb_filter(hObject, eventdata, handles)
handles.output = hObject;
handles.current_rgb = handles.current_img;
ranges = [handles.R_min, handles.R_max, handles.G_min, handles.G_max, handles.B_min, handles.B_max];
for i=1:3
    for k=1:3
        handles.current_rgb(:,:,k) = handles.current_rgb(:,:,k).*uint8(inrange(handles.current_img(:,:,i), ranges(2*i-1), ranges(2*i)));
    end
end
axes(handles.axes4);
imshow(handles.current_rgb);
guidata(hObject, handles);


function Cek(hObject, eventdata, handles)
handles.output = hObject;
n = length(handles.files);
if handles.index > 1,  set(handles.pushbutton1,'enable','on');
else                           set(handles.pushbutton1,'enable','off'); end
if handles.index < n, set(handles.pushbutton2,'enable','on');
else                           set(handles.pushbutton2,'enable','off'); end
guidata(hObject, handles);

function R_min_slider_Callback(hObject, eventdata, handles)
handles.R_min = round(get(hObject,'Value'));
rgb_filter(hObject, eventdata, handles);
set(handles.R_min_edit,'String',int2str(handles.R_min));
guidata(hObject, handles);

function R_min_slider_CreateFcn(hObject, eventdata, handles)
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end

function R_min_edit_Callback(hObject, eventdata, handles)
handles.R_min = round(str2num(get(hObject,'String')));
rgb_filter(hObject, eventdata, handles);
set(handles.R_min_slider, 'Value', handles.R_min);
guidata(hObject, handles);


function R_min_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function slider4_Callback(hObject, eventdata, handles)

function slider4_CreateFcn(hObject, eventdata, handles)
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end

function G_min_slider_Callback(hObject, eventdata, handles)
handles.G_min = round(get(hObject,'Value'));
rgb_filter(hObject, eventdata, handles);
set(handles.G_min_edit,'String',int2str(handles.G_min));
guidata(hObject, handles);

function G_min_slider_CreateFcn(hObject, eventdata, handles)
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end

function G_max_slider_Callback(hObject, eventdata, handles)
handles.G_max = round(get(hObject,'Value'));
rgb_filter(hObject, eventdata, handles);
set(handles.G_max_edit,'String',int2str(handles.G_max));
guidata(hObject, handles);

function G_max_slider_CreateFcn(hObject, eventdata, handles)
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end

function B_min_slider_Callback(hObject, eventdata, handles)
handles.B_min = round(get(hObject,'Value'));
rgb_filter(hObject, eventdata, handles);
set(handles.B_min_edit,'String',int2str(handles.B_min));
guidata(hObject, handles);

function B_min_slider_CreateFcn(hObject, eventdata, handles)
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end

function R_max_edit_Callback(hObject, eventdata, handles)
handles.R_max = round(str2num(get(hObject,'String')));
rgb_filter(hObject, eventdata, handles);
set(handles.R_max_slider, 'Value', handles.R_max);
guidata(hObject, handles);

function R_max_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function G_min_edit_Callback(hObject, eventdata, handles)
handles.G_min = round(str2num(get(hObject,'String')));
rgb_filter(hObject, eventdata, handles);
set(handles.G_min_slider, 'Value', handles.G_min);
guidata(hObject, handles);

function G_min_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function G_max_edit_Callback(hObject, eventdata, handles)
handles.G_max = round(str2num(get(hObject,'String')));
rgb_filter(hObject, eventdata, handles);
set(handles.G_max_slider, 'Value', handles.G_max);
guidata(hObject, handles);

function G_max_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function B_min_edit_Callback(hObject, eventdata, handles)
handles.B_min = round(str2num(get(hObject,'String')));
rgb_filter(hObject, eventdata, handles);
set(handles.B_min_slider, 'Value', handles.B_min);
guidata(hObject, handles);

function B_min_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function edit6_Callback(hObject, eventdata, handles)

function edit6_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function edit7_Callback(hObject, eventdata, handles)

function edit7_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function edit8_Callback(hObject, eventdata, handles)

function edit8_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function edit9_Callback(hObject, eventdata, handles)

function edit9_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function edit10_Callback(hObject, eventdata, handles)

function edit10_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function edit11_Callback(hObject, eventdata, handles)

function edit11_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function slider9_Callback(hObject, eventdata, handles)

function slider9_CreateFcn(hObject, eventdata, handles)
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end

function slider10_Callback(hObject, eventdata, handles)

function slider10_CreateFcn(hObject, eventdata, handles)
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end

function slider11_Callback(hObject, eventdata, handles)

function slider11_CreateFcn(hObject, eventdata, handles)
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end

function slider12_Callback(hObject, eventdata, handles)

function slider12_CreateFcn(hObject, eventdata, handles)
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end

function slider13_Callback(hObject, eventdata, handles)

function slider13_CreateFcn(hObject, eventdata, handles)
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end

function slider14_Callback(hObject, eventdata, handles)

function slider14_CreateFcn(hObject, eventdata, handles)
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end

function edit12_Callback(hObject, eventdata, handles)

function edit12_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function R_max_slider_Callback(hObject, eventdata, handles)
handles.R_max = round(get(hObject,'Value'));
rgb_filter(hObject, eventdata, handles);
set(handles.R_max_edit,'String',int2str(handles.R_max));
guidata(hObject, handles);

function R_max_slider_CreateFcn(hObject, eventdata, handles)
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end

function B_max_edit_Callback(hObject, eventdata, handles)
handles.B_max = round(str2num(get(hObject,'String')));
rgb_filter(hObject, eventdata, handles);
set(handles.B_max_slider, 'Value', handles.B_max);
guidata(hObject, handles);

function B_max_edit_CreateFcn(hObject, eventdata, handles)

if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on slider movement.
function B_max_slider_Callback(hObject, eventdata, handles)
handles.B_max = round(get(hObject,'Value'));
rgb_filter(hObject, eventdata, handles);

set(handles.B_max_edit,'String',int2str(handles.B_max));
guidata(hObject, handles);

function B_max_slider_CreateFcn(hObject, eventdata, handles)
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end
