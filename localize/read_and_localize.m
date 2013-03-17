function read_and_localize(image_dir,gt_index_,list_file_)

% list of images, directory, result

if nargin == 0
image_dir = '../../data/Dubrovnik6K/';
list_file_ = '../../data/Dubrovnik6K/bundle/list.orig.txt'
gt_index_ = '../generate_gt/dubrovnik.gt'
end



% ----------read image list -------------------

fid = fopen(list_file_);
list = textscan(fid,'%s\n');
image_list = list{1};


%-----------read gt index file ----------------
gt_index=dlmread(gt_index_,' ');


num_query = size(gt_index,1);
num_ret = size(gt_index,2);
for i = 1:num_query
    query_image=image_list(gt_index(i,1)+1);
    query_image_name=textscan(query_image{1},'query/%s');
    
    
    
    for j = 2:num_ret
        db_image=image_list(gt_index(i,j)+1);
        db_image_name = textscan(db_image{1},'db/%s');
    
        query_image_loc = [image_dir,'query_images/',query_image_name{1}{1}];
        db_image_loc = [image_dir,'db_images/',db_image_name{1}{1}];
        
        % localize the image
        localize(query_image_loc,db_image_loc);
    end
end

end