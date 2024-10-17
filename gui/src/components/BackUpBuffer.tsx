import { RootState } from '@/stores/store';
import React from 'react';
import { useSelector } from 'react-redux';

const BackUpBuffer = () => {
  const selectedFile = useSelector((state: RootState) => state.createCommit.selectedFile);
  return (
    <div>
      <h1>BackUpBuffer</h1>
      <div>
        {selectedFile.map(path => (
          <div key={path}>{path}</div>
        ))}
      </div>
    </div>
  );
};

export default BackUpBuffer;
