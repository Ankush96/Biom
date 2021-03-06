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
handles.H_val = 0;
handles.H_tol = 90;
handles.S_min = 0;
handles.S_max = 255;
handles.V_min = 0;
handles.V_max = 255;

axes(handles.axes3);
imshow(handles.current_ycbcr);
handles.Cr_min = 0;
handles.Cr_max = 255;
handles.Cb_min = 0;
handles.Cb_max = 255;

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
hsv_filter(hObject, eventdata, handles);
ycbcr_filter(hObject, eventdata, handles);
guidata(hObject, handles);

function pushbutton2_Callback(hObject, eventdata, handles)
handles.output = hObject;
handles.index = handles.index + 1;
axes(handles.axes1);
Cek(hObject, eventdata, handles);
handles.current_img = imread(handles.X{handles.index});
imshow(handles.current_img);
rgb_filter(hObject, eventdata, handles);
hsv_filter(hObject, eventdata, handles);
ycbcr_filter(hObject, eventdata, handles);
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

function hsv_filter(hObject, eventdata, handles)
handles.output = hObject;
handles.current_hsv = handles.current_img;
h = im2uint8(rgb2hsv(handles.current_img));
v = handles.H_val;
t = handles.H_tol;
ranges = [handles.S_min, handles.S_max, handles.V_min, handles.V_max];
for i=2:3
    for k=1:3
        handles.current_hsv(:,:,k) = handles.current_hsv(:,:,k).*uint8(inrange(h(:,:,i), ranges(2*i-3), ranges(2*i-2)));
    end
end
if (v-t)>=0 && (v+t)<=180
    for k=1:3
        handles.current_hsv(:,:,k) = handles.current_hsv(:,:,k).*uint8(inrange(h(:,:,1), v-t, v+t));
    end
elseif (v-t)<0
    for k=1:3
        handles.current_hsv(:,:,k) = handles.current_hsv(:,:,k).*uint8(inrange(h(:,:,1), 0, v+t)|inrange(h(:,:,1), mod(v-t,180), 180));
    end
elseif (v+t)>180   
    for k=1:3
        handles.current_hsv(:,:,k) = handles.current_hsv(:,:,k).*uint8(inrange(h(:,:,1), 0, mod(v+t,180))|inrange(h(:,:,1), v-t, 180));
    end
end
axes(handles.axes2);
imshow(handles.current_hsv);
guidata(hObject, handles);

function ycbcr_filter(hObject, eventdata, handles)
handles.output = hObject;
handles.current_ycbcr = handles.current_img;
ycc = rgb2ycbcr(handles.current_img);
ranges = [handles.Cb_min, handles.Cb_max, handles.Cr_min, handles.Cr_max];
for i=2:3
    for k=1:3
        handles.current_ycbcr(:,:,k) = handles.current_ycbcr(:,:,k).*uint8(inrange(ycc(:,:,i), ranges(2*i-3), ranges(2*i-2)));
    end
end
axes(handles.axes3);
imshow(handles.current_ycbcr);
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

function Cb_min_edit_Callback(hObject, eventdata, handles)
handles.Cb_min = round(str2num(get(hObject,'String')));
ycbcr_filter(hObject, eventdata, handles);
set(handles.Cb_min_slider, 'Value', handles.Cb_min);
guidata(hObject, handles);

function Cb_min_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function Cb_max_edit_Callback(hObject, eventdata, handles)
handles.Cb_max = round(str2num(get(hObject,'String')));
ycbcr_filter(hObject, eventdata, handles);
set(handles.Cb_max_slider, 'Value', handles.Cb_max);
guidata(hObject, handles);

function Cb_max_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function Cr_min_edit_Callback(hObject, eventdata, handles)
handles.Cr_min = round(str2num(get(hObject,'String')));
ycbcr_filter(hObject, eventdata, handles);
set(handles.Cr_min_slider, 'Value', handles.Cr_min);
guidata(hObject, handles);

function Cr_min_edit_CreateFcn(hObject, eventdata, handles)
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

function Cb_min_slider_Callback(hObject, eventdata, handles)
handles.Cb_min = round(get(hObject,'Value'));
ycbcr_filter(hObject, eventdata, handles);
set(handles.Cb_min_edit,'String',int2str(handles.Cb_min));
guidata(hObject, handles);

function Cb_min_slider_CreateFcn(hObject, eventdata, handles)
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end

function slider12_Callback(hObject, eventdata, handles)

function slider12_CreateFcn(hObject, eventdata, handles)
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end

function Cr_min_slider_Callback(hObject, eventdata, handles)
handles.Cr_min = round(get(hObject,'Value'));
ycbcr_filter(hObject, eventdata, handles);
set(handles.Cr_min_edit,'String',int2str(handles.Cr_min));
guidata(hObject, handles);

function Cr_min_slider_CreateFcn(hObject, eventdata, handles)
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end

function Cr_max_slider_Callback(hObject, eventdata, handles)
handles.Cr_max = round(get(hObject,'Value'));
ycbcr_filter(hObject, eventdata, handles);
set(handles.Cr_max_edit,'String',int2str(handles.Cr_max));
guidata(hObject, handles);

function Cr_max_slider_CreateFcn(hObject, eventdata, handles)
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end

function Cr_max_edit_Callback(hObject, eventdata, handles)
handles.Cr_max = round(str2num(get(hObject,'String')));
ycbcr_filter(hObject, eventdata, handles);
set(handles.Cr_max_slider, 'Value', handles.Cr_max);
guidata(hObject, handles);

function Cr_max_edit_CreateFcn(hObject, eventdata, handles)
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


% --- Executes on slider movement.
function Cb_max_slider_Callback(hObject, eventdata, handles)
handles.Cb_max = round(get(hObject,'Value'));
ycbcr_filter(hObject, eventdata, handles);

set(handles.Cb_max_edit,'String',int2str(handles.Cb_max));
guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function Cb_max_slider_CreateFcn(hObject, eventdata, handles)
% hObject    handle to Cb_max_slider (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end



function H_val_edit_Callback(hObject, eventdata, handles)
handles.H_val = round(str2num(get(hObject,'String')));
hsv_filter(hObject, eventdata, handles);
set(handles.H_val_slider, 'Value', handles.H_val);
guidata(hObject, handles);

function H_val_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function vvvv_CreateFcn(hObject, eventdata, handles)
% hObject    handle to vvvv (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function H_tol_edit_Callback(hObject, eventdata, handles)
handles.H_tol = round(str2num(get(hObject,'String')));
hsv_filter(hObject, eventdata, handles);
set(handles.H_tol_slider, 'Value', handles.H_tol);
guidata(hObject, handles);

function H_tol_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function S_min_edit_Callback(hObject, eventdata, handles)
handles.S_min = round(str2num(get(hObject,'String')));
hsv_filter(hObject, eventdata, handles);
set(handles.S_min_slider, 'Value', handles.S_min);
guidata(hObject, handles);

function S_min_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function S_max_edit_Callback(hObject, eventdata, handles)
handles.S_max = round(str2num(get(hObject,'String')));
hsv_filter(hObject, eventdata, handles);
set(handles.S_max_slider, 'Value', handles.S_max);
guidata(hObject, handles);

function S_max_edit_CreateFcn(hObject, eventdata, handles)

if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function V_min_edit_Callback(hObject, eventdata, handles)
handles.V_min = round(str2num(get(hObject,'String')));
hsv_filter(hObject, eventdata, handles);
set(handles.V_min_slider, 'Value', handles.V_min);
guidata(hObject, handles);

function V_min_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on slider movement.
function H_val_slider_Callback(hObject, eventdata, handles)
handles.H_val = round(get(hObject,'Value'));
hsv_filter(hObject, eventdata, handles);
set(handles.H_val_edit,'String',int2str(handles.H_val));
guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function H_val_slider_CreateFcn(hObject, eventdata, handles)
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end


% --- Executes on slider movement.
function S_min_slider_Callback(hObject, eventdata, handles)
handles.S_min = round(get(hObject,'Value'));
hsv_filter(hObject, eventdata, handles);
set(handles.S_min_edit,'String',int2str(handles.S_min));
guidata(hObject, handles);

function S_min_slider_CreateFcn(hObject, eventdata, handles)
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end


% --- Executes on slider movement.
function S_max_slider_Callback(hObject, eventdata, handles)
handles.S_max = round(get(hObject,'Value'));
hsv_filter(hObject, eventdata, handles);
set(handles.S_max_edit,'String',int2str(handles.S_max));
guidata(hObject, handles);

function S_max_slider_CreateFcn(hObject, eventdata, handles)
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end


% --- Executes on slider movement.
function V_min_slider_Callback(hObject, eventdata, handles)
handles.V_min = round(get(hObject,'Value'));
hsv_filter(hObject, eventdata, handles);
set(handles.V_min_edit,'String',int2str(handles.V_min));
guidata(hObject, handles);

function V_min_slider_CreateFcn(hObject, eventdata, handles)
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end


% --- Executes on slider movement.
function H_tol_slider_Callback(hObject, eventdata, handles)
handles.H_tol = round(get(hObject,'Value'));
hsv_filter(hObject, eventdata, handles);
set(handles.H_tol_edit,'String',int2str(handles.H_tol));
guidata(hObject, handles);

function H_tol_slider_CreateFcn(hObject, eventdata, handles)
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end



function V_max_edit_Callback(hObject, eventdata, handles)
handles.V_max = round(str2num(get(hObject,'String')));
hsv_filter(hObject, eventdata, handles);
set(handles.V_max_slider, 'Value', handles.V_max);
guidata(hObject, handles);

function V_max_edit_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on slider movement.
function V_max_slider_Callback(hObject, eventdata, handles)
handles.V_max = round(get(hObject,'Value'));
hsv_filter(hObject, eventdata, handles);
set(handles.V_max_edit,'String',int2str(handles.V_max));
guidata(hObject, handles);

function V_max_slider_CreateFcn(hObject, eventdata, handles)
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end
