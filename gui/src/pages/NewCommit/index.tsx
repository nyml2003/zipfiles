import React from 'react';
import FileList from './FileList';
import BackupOption from './BackupOption';

const NewCommit: React.FC = () => {
  return (
    <div className='w-full h-full flex flex-col bg-white'>
      <FileList />
      <BackupOption />
    </div>
  );
};

export default NewCommit;
